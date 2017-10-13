#pragma once

#include <string>

#include "Socket.h"

class Session
{
public:
  Session(std::string host, int port);
  ~Session();

protected:
private:
  Socket teste;
  // Socket receiver;
  // Socket sender;
};