#include "Session.h"

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

#include <cstring>
#include <fstream>

#include <utime.h> // to set manually the last modified timestamp
#include <sys/stat.h>

#include "DirectoryManager.h"
#include "Utilities.h"

Session::Session(std::string host, int port) :
alive(true),
directoryManager(nullptr)
{
  socket = new Socket(host, port);
  socket->connect();

  // initializes read thread
  startListening();
  
}

Session::Session(Socket *s) :
socket(s),
alive(true),
directoryManager(nullptr)
{
  // initializes read thread
  startListening();
}


Session::~Session()
{
  if(this->listenThread) {
    stopListening();
    delete this->listenThread;
  }

  delete socket;
}


Socket* Session::getSocket()
{
  return this->socket;
}


bool Session::isAlive()
{
  return this->alive;
}


void Session::listen()
{
  std::cout << "Listening..." << std::endl;
  this->_isListening = true;
  Utilities::Message message;
  int response = 0;
  while(_isListening)
  {
    // std::cout << "Back on Listening..." << std::endl;
    // locks the mutex
    // so we can use the socket without any interference
    socket_mtx.lock();
    response = socket->read(reinterpret_cast<char*>(&message), 
                                sizeof(message));
    socket_mtx.unlock();

    if(response > 0) {
      std::cout << "Received message from Socket "
                << "(" << this->socket << "): "
                << std::endl;
      this->onMessage(message);
    }
    else {
      if(response == 0) {
        std::cout << "Socket "
                  << "(" << this->socket << "): "
                  << "connection closed (0 bytes read)"
                  << std::endl;
        _isListening = false;
        this->alive = false;
      }
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}


void Session::startListening()
{
  if(listenThread == nullptr) { // prevents multiple listen threads and mem leak
    this->listenThread = new std::thread(&Session::listen, this);
  }
}

void Session::stopListening()
{
  this->_isListening = false;
  listenThread->join();
}


void* Session::request(Utilities::Message request)
{
  std::cout << "Requesting " << request.field << std::endl;

  // we need to serialize the structure:
  // (we do a reinterpret cast when sending
  // to tell the compiler to interpret the message
  // as a byte array) - and bob's your uncle!

  // locks the mutex
  // so we can use the socket without any interference
  socket_mtx.lock();

  this->socket->write(reinterpret_cast<char*>(&request),
                      sizeof(request));

  std::cout << "Request sent!" << std::endl;
  std::cout << "Waiting for reply..." << std::endl;
  
  Utilities::Message *response = new Utilities::Message();
  int status = 0;
  if((status = socket->read(reinterpret_cast<char*>(response), 
                              sizeof(Utilities::Message))) > 0) {
      std::cout << "Received response from Socket "
                << "(" << this->socket << "): "
                << std::endl;
    }

  socket_mtx.unlock();
  
  return response;
}

void Session::reply(Utilities::Message replyMessage) 
{
  socket_mtx.lock();
  // std::cout << "Sending reply" << std::endl;
  this->socket->write(reinterpret_cast<char*>(&replyMessage),
                      sizeof(replyMessage));
  // std::cout << "Reply sent!" << std::endl;
  socket_mtx.unlock();
}

void Session::sendFile(std::string filepath) 
{
  std::cout << "Sending file: " << filepath << std::endl;

  // to-do: break down filepath to send only file name
  size_t pos = filepath.find_last_of("/\\");
  std::string fileName = filepath.substr(pos+1);


  Utilities::Message replyMessage { Utilities::REPLY, 
                                    Utilities::BEGIN_OF_FILE, 
                                    fileName.size() };

  struct stat file_stat;

  if(stat(filepath.c_str(), &file_stat) != -1) {
    replyMessage.fileSize = (int) file_stat.st_size;
    replyMessage.lastModified = file_stat.st_mtime;
  }

  // "Hey, we are sending a file
  // and its name is..."
  fileName.copy(replyMessage.content, fileName.size());
  this->reply(replyMessage);


  // opens the file and starts build blocks to send
  std::ifstream infile;
  infile.open(filepath, std::ios::binary | std::ios::in);

  if (infile.is_open()) {
    char *fileContent = new char[FILE_BLOCK_SIZE]();
    int offset = 0;

    // Keeps sending
    // full 256 byte blocks of data
    // until reaches end of file
    while (!infile.eof())
    {
      // reads char by char until we reach block size
      infile.read(fileContent + offset, 1);
      offset++;

      if (offset == FILE_BLOCK_SIZE) {
        offset = 0;
        replyMessage = { Utilities::REPLY, 
                         Utilities::CONTENT_OF_FILE,
                         FILE_BLOCK_SIZE};
        memcpy(replyMessage.content, fileContent, FILE_BLOCK_SIZE);
        // send the 256 bytes block
        this->reply(replyMessage);
      }
    }

    // remove eof char
    offset--;

    // if we reach the end of file
    // but there are still bytes to send
    // (filesize % blocksize) bytes remaining

    // Send remaining bytes from file
    if (offset > 0) {
      replyMessage = {Utilities::REPLY, Utilities::CONTENT_OF_FILE, offset};
      memcpy(replyMessage.content, fileContent, offset);
      this->reply(replyMessage);
    }
    
    delete[] fileContent;
  }
  infile.close();


  // "Hey, the file is over"
  replyMessage = { Utilities::REPLY, 
                   Utilities::END_OF_FILE, 
                   fileName.size() };
  fileName.copy(replyMessage.content, fileName.size());
  this->reply(replyMessage);
}

void Session::saveFile() 
{
  bool shouldSave = true;
  bool shouldSync = false;


  std::string filepath;

  if (this->downloadToCurrDir) {
    filepath = this->fileName;
    this->downloadToCurrDir = false;
  }
  else {
    filepath = this->directoryManager->getPath() + "/" + this->fileName;
  }

  // first, we check if we already have
  // the file in sync_dir
  struct stat file_stat;                       
  if(stat(filepath.c_str(), &file_stat) == 0) {
    std::cout << "Found a file with same name" << std::endl;
    std::cout << "Local file last modified at: " 
              << asctime(localtime(&file_stat.st_mtime))
              << std::endl;

    std::cout << "Received file last modified at: "
              << asctime(localtime(&(this->fileLastModified)))
              << std::endl;

    // there is a file with this name
    // now, we must check the file's modification info
    if(difftime(file_stat.st_mtime, this->fileLastModified) > 0) {
      std::cout << "Local file is newer than received one" 
                << std::endl;
      // local file is newer, shouldn't save
      // and should sync with remote
      shouldSave = false;
      shouldSync = true;
    }
    else if(difftime(file_stat.st_mtime, this->fileLastModified) == 0) {
      std::cout << "Local file and received one are the same" 
                << std::endl;
      // local and remote file are the same
      shouldSave = false;
    }

  }


  if(shouldSave) {
    std::cout << "Saving file: " << this->fileName 
              << " at : " << filepath
              << std::endl;

    utimbuf access_modified_times = { file_stat.st_atime, 
                                      this->fileLastModified };

    std::ofstream outfile;
    outfile.open(filepath, std::ofstream::binary);

    if (outfile.is_open()) {
      outfile.write((const char*) &file[0], file.size());
      outfile.close();
      utime(filepath.c_str(), &access_modified_times);
    } 
  }
  

  if(shouldSync) {
    // when the local file is newer than the one received
    // we upload so that the other endpoint can have the most-recent
    // file as well
    this->sendFile(filepath);
  }

}

void Session::requestDeleteFile(std::string filepath)
{
  size_t pos = filepath.find_last_of("/\\");
  std::string fileName = filepath.substr(pos+1);

  Utilities::Message message { Utilities::REQUEST, 
                               Utilities::DELETE_FILE, 
                               fileName.size() };

  fileName.copy(message.content, fileName.size());
  this->request(message);
}

void Session::deleteFile(std::string fileName)
{
  std::string filepath = this->directoryManager->getPath()
                       + "/"
                       + this->fileName;

  if (remove(filepath.c_str()) == 0) 
    std::cout << "File " << fileName << " deleted" << std::endl;
}