#pragma once

#include "Session.h"

#include <string>

#include "Socket.h"
#include "Utilities.h"


class ServerSession : public Session
{
public:
  ServerSession(Socket *s);
  ~ServerSession();

protected:
private:
  void* onMessage(Utilities::Message message) override;
  
};