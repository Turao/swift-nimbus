#pragma once

#include "Session.h"

#include <string>

#include "Utilities.h"


class ClientSession : public Session
{
public:
  ClientSession(std::string host, int port);
  ~ClientSession();

protected:
private:
  void* onMessage(Utilities::Message message) override;
  
};