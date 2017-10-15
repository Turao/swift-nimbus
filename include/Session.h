#pragma once

#include <string>

#include "Socket.h"

#include <thread>
#include <atomic>

class Session
{
public:
  Session(std::string host, int port);
  Session(Socket *s);
  ~Session();

  Socket* getSocket();
  bool isAlive();

  void stopTailThread(); //just testing... TO-DO: remove
  void stopWriterThread(); //just testing... TO-DO: remove


protected:
private:
  // thread pattern below
  // https://stackoverflow.com/questions/37358597/start-a-daemon-thread-thats-a-private-method-of-the-class
  std::atomic<bool> _tail_isRunning;
  std::thread *tailThread;
  void tail();


  std::atomic<bool> _writer_isRunning;
  std::thread *writerThread;
  void writer();


  Socket *socket;
  std::atomic<bool> alive;
};