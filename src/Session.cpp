#include "Session.h"

#include <iostream>
#include <thread>


#include <chrono>

Session::Session(std::string host, int port) :
alive(true)
{
  socket = new Socket(host, port);
  socket->connectSocket();

  // initializes read thread
  this->writerThread = std::thread(&Session::writer, this);
  while(1)
  {
    std::this_thread::sleep_for (std::chrono::seconds(1));
  }
}

Session::Session(Socket *s) :
socket(s),
alive(true)
{
  // initializes read thread
  this->tailThread = std::thread(&Session::tail, this);
}


Session::~Session()
{
  if(this->_tail_isRunning) {
    std::cout << "Stopping session threads (tail)" << std::endl;
    stopTailThread();
  }

  if(this->_writer_isRunning) {
    std::cout << "Stopping session threads (writer)" << std::endl;
    stopWriterThread();
  }

  std::cout << "Deleting socket" << std::endl;
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
        _tail_isRunning = false;
        std::cout << "Socket "
                  << "(" << this->socket << "): "
                  << "connection closed (0 bytes read)"
                  << std::endl;
        this->alive = false;
      }
    }
  }
}


void Session::stopTailThread() //just testing...
{
  this->_tail_isRunning = false;
  tailThread.join();
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

void Session::stopWriterThread() //just testing...
{
  this->_writer_isRunning = false;
  writerThread.join();
}