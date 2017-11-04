#include "Session.h"

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

#include <cstring>
#include <fstream>

#include "Utilities.h"

Session::Session(std::string host, int port) :
alive(true)
{
  socket = new Socket(host, port);
  socket->connect();

  // initializes read thread
  startListening();
  
}

Session::Session(Socket *s) :
socket(s),
alive(true)
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

void Session::sendFile(const char *fileName) 
{
  std::cout << "Sending file: " << fileName << std::endl;


  Utilities::Message replyMessage { Utilities::REPLY, 
                                    Utilities::BEGIN_OF_FILE, 
                                    strlen(fileName) };
  

  // "Hey, we are sending a file
  // and its name is..."
  strcpy(replyMessage.content, fileName);
  this->reply(replyMessage);


  // opens the file and starts build blocks to send
  std::ifstream infile;
  infile.open(fileName, std::ios::binary | std::ios::in);

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
                   strlen(fileName) };
  strcpy(replyMessage.content, fileName);
  this->reply(replyMessage);
}

void Session::saveFile() 
{
  std::ofstream outfile;
  outfile.open("testes.txt", std::ofstream::binary); // to-do: change filename

  if (outfile.is_open()) {
    outfile.write((const char*) &file[0], file.size());
    outfile.close();
  }

}