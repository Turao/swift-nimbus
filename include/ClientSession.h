#pragma once

#include "Session.h"

#include <string>

#include "Socket.h"


class ClientSession : public Session
{
public:
  ClientSession(std::string host, int port);
  ~ClientSession();

protected:
private:
  void* onMessage(std::string message) override;
  
};