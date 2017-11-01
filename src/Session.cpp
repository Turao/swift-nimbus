#include "Session.h"

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

#include <cstring>

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


    if((response = socket->read(reinterpret_cast<char*>(&message), 
                                sizeof(message))) > 0) {
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


void* Session::request(Utilities::Field field)
{
  std::cout << "Requesting " << field << std::endl;
  
  // prepare request
  Utilities::Message request {Utilities::REQUEST, field};

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
    }

  socket_mtx.unlock();
  
  return nullptr; //to-do: return response
}