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
  this->_isListening = true;
  Utilities::Message message;
  int response = 0;
  while(_isListening)
  {
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

  // serializes structure
  // (we do a reinterpret cast when sending
  // to tell the compiler to interpret the message
  // as a byte array)

  // locks the mutex
  // so we can use the socket without any interference
  socket_mtx.lock();

  this->socket->write(reinterpret_cast<char*>(&request),
                      sizeof(request));

  std::cout << "Request sent!" << std::endl;
  std::cout << "Waiting for reply..." << std::endl;
  
  Utilities::Message response;
  int status = 0;
  if((status = socket->read(reinterpret_cast<char*>(&response), 
                              sizeof(response))) > 0) {
      std::cout << "Received response from Socket "
                << "(" << this->socket << "): "
                << std::endl;
      this->handleReply(response);
    }

  socket_mtx.unlock();
  
  return nullptr; //to-do: return response
}

void Session::reply(Utilities::Message replyMessage) 
{
  socket_mtx.lock();
  std::cout << "Sending reply" << std::endl;
  this->socket->write(reinterpret_cast<char*>(&replyMessage),
                      sizeof(replyMessage));
  socket_mtx.unlock();
}

void Session::sendFile(const char *fileName) 
{
  std::cout << "Sending file: " << fileName << std::endl;
  Utilities::Message replyMessage {Utilities::REPLY, Utilities::BEGIN_OF_FILE, strlen(fileName)};
  char *fileContent = new char[FILE_BLOCK_SIZE]();
  int offset = 0;

  // Hey, we are sending a file

  strcpy(replyMessage.content, fileName);
  this->reply(replyMessage);


  std::ifstream infile;
  infile.open(fileName, std::ios::binary | std::ios::in);

  if (infile.is_open()) {

    // Send full 256 byte blocks from file

    while (!infile.eof()) {
      infile.read(fileContent + offset, 1);
      offset++;
      if (offset == FILE_BLOCK_SIZE)
      {
        offset = 0;
        replyMessage = {Utilities::REPLY, Utilities::CONTENT_OF_FILE, FILE_BLOCK_SIZE};
        memcpy(replyMessage.content, fileContent, FILE_BLOCK_SIZE);
        this->reply(replyMessage);
      }
    }

    // Send remaining bytes from file

    if (offset > 0) {
      replyMessage = {Utilities::REPLY, Utilities::CONTENT_OF_FILE, offset};
      memcpy(replyMessage.content, fileContent, offset);
      this->reply(replyMessage);
    }
  }

  // Hey, the file is over

  replyMessage = {Utilities::REPLY, Utilities::END_OF_FILE, strlen(fileName)};
  strcpy(replyMessage.content, fileName);
  this->reply(replyMessage);

  infile.close();
  delete[] fileContent;
}