#include "Session.h"

#include <iostream>
#include <thread>


#include <chrono>

Session::Session(std::string host, int port)
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
socket(s)
{
  // initializes read thread
  this->tailThread = std::thread(&Session::tail, this);
  while(1)
  {
    std::this_thread::sleep_for (std::chrono::seconds(1));
  }
}


Session::~Session()
{

}

Socket* Session::getSocket()
{
  return this->socket;
}

void Session::tail() //just testing...
{
  this->_tail_isRunning = true;
  char buffer[256];
  while(1)
  {
    if(socket->read(buffer, sizeof(buffer)) > 0)
      std::cout << buffer << std::endl;
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
  while(1)
  {
    std::cin >> buffer;
    socket->write(buffer, sizeof(buffer));
  }

}

void Session::stopWriterThread() //just testing...
{
  this->_writer_isRunning = false;
  writerThread.join();
}