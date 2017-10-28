#include "Session.h"

#include <iostream>
#include <thread>


#include <chrono>

Session::Session(std::string host, int port) :
alive(true),
tailThread(nullptr),
writerThread(nullptr)
{
  socket = new Socket(host, port);
  socket->connect();

  // initializes read thread
  startWriterThread();
  while(1)
  {
    std::this_thread::sleep_for (std::chrono::seconds(1));
  }
}

Session::Session(Socket *s) :
socket(s),
alive(true),
tailThread(nullptr),
writerThread(nullptr)
{
  // initializes read thread
  startTailThread();
}


Session::~Session()
{
  if(this->tailThread) {
    stopTailThread();
    delete this->tailThread;
  }

  if(this->writerThread) {
    stopWriterThread();
    delete this->writerThread;
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


void Session::tail() //just testing...
{
  this->_tail_isRunning = true;
  char buffer[256];
  int response = 0;
  while(_tail_isRunning)
  {
    if((response = socket->read(buffer, sizeof(buffer))) > 0) {
      std::cout << "Received from Socket "
                << "(" << this->socket << "): "
                << buffer
                << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    else {
      if(response == 0) {
        std::cout << "Socket "
                  << "(" << this->socket << "): "
                  << "connection closed (0 bytes read)"
                  << std::endl;
        _tail_isRunning = false;
        this->alive = false;
      }
    }
  }
}


void Session::startTailThread() //just testing...
{
  if(tailThread == nullptr) { // prevents multiple tail threads and mem leak
    this->tailThread = new std::thread(&Session::tail, this);
  }
}

void Session::stopTailThread() //just testing...
{
  this->_tail_isRunning = false;
  tailThread->join();
}



void Session::writer() //just testing...
{
  this->_writer_isRunning = true;
  char buffer[256];
  while(_writer_isRunning)
  {
    std::cin >> buffer;
    socket->write(buffer, sizeof(buffer));
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

}

void Session::startWriterThread() //just testing...
{
  if(tailThread == nullptr) { // prevents multiple writer threads and mem leak
    this->writerThread = new std::thread(&Session::writer, this);
  }
}

void Session::stopWriterThread() //just testing...
{
  this->_writer_isRunning = false;
  writerThread->join();
}