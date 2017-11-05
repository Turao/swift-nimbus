#pragma once

#include "Session.h"

#include <string>

#include "Utilities.h"


class ClientSession : public Session
{
public:
  ClientSession(std::string host, int port, std::string username);
  ~ClientSession();

protected:
private:  
  std::string username;

  void* onMessage(Utilities::Message message) override;
  void handleRequest(Utilities::Message message);
  void handleReply(Utilities::Message message);
  
};