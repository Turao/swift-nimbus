#pragma once

#include <string>

class Socket
{
public:
  Socket(std::string host, int port);
  ~Socket();
};