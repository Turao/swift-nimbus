#pragma once

#include <string>

#include "Socket.h"

class Session
{
public:
  Session(std::string host, int port);
  ~Session();

  Socket getSocket();

protected:
private:
  Socket teste;
  // Socket receiver;
  // Socket sender;
};