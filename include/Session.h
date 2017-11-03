#pragma once

#include <string>

#include "Socket.h"

#include <thread>
#include <atomic>
#include <mutex>

#include "Utilities.h"

class Session
{
public:
  Session(std::string host, int port);
  Session(Socket *s);
  ~Session();

  Socket* getSocket();
  bool isAlive();

  void startListening();
  void stopListening();

  void* request(Utilities::Message request);
  void reply(Utilities::Message message);
  void sendFile(const char *fileName);

protected:

private:
  // thread pattern below
  // https://stackoverflow.com/questions/37358597/start-a-daemon-thread-thats-a-private-method-of-the-class
  std::atomic<bool> _isListening;
  std::thread *listenThread = nullptr;
  void listen();

  Socket *socket;
  std::atomic<bool> alive;

  std::mutex socket_mtx;
  virtual  void* onMessage(Utilities::Message message) = 0;
  virtual  void handleReply(Utilities::Message message) = 0;
  virtual  void handleRequest(Utilities::Message message) = 0;
};