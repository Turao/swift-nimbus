#include "Session.h"

#include <iostream>
#include <thread>
#include <mutex>



#include <chrono>

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
  char buffer[256];
  int response = 0;
  while(_isListening)
  {
    // locks the mutex
    // so we can use the socket without any interference
    socket_mtx.lock();


    if((response = socket->read(buffer, sizeof(buffer))) > 0) {
      std::cout << "Received from Socket "
                << "(" << this->socket << "): "
                << buffer
                << std::endl;
      this->onMessage(buffer);
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
    socket_mtx.unlock();

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


void* Session::request(std::string field)
{
  std::cout << "Requesting: " << field << std::endl;
  
  // to-do: prepare request packet
  // to-do: serializes packet

  std::string req = "request: " + field;

  // locks the mutex
  // so we can use the socket without any interference
  socket_mtx.lock();

  this->socket->write(const_cast<char*>(req.c_str()),
                      req.size());

  std::cout << "Request sent!" << std::endl;
  std::cout << "Waiting for reply..." << std::endl;
  
  char buffer[256];
  int response = 0;
  if((response = socket->read(buffer, sizeof(buffer))) > 0) {
      std::cout << "Received from Socket "
                << "(" << this->socket << "): "
                << buffer
                << std::endl;
    }

  socket_mtx.unlock();
  
  return nullptr; //to-do: return response
}