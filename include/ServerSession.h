#pragma once

#include "Session.h"

#include <string>

#include "Socket.h"


class ServerSession : public Session
{
public:
  ServerSession(Socket *s);
  ~ServerSession();

protected:
private:
  void* onMessage(std::string message) override;
  
};