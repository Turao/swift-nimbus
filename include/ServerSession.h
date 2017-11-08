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
  std::string username;

  void* onMessage(Utilities::Message message) override;
  void handleReply(Utilities::Message message);
  void handleRequest(Utilities::Message message);
  void sendFilesList();
  
};