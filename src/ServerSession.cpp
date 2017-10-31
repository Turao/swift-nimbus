#include "ServerSession.h"

#include <iostream>
#include <string>

ServerSession::ServerSession(Socket *s) :
Session(s)
{
  std::cout << "Initializing Server-side session" << std::endl;
}


ServerSession::~ServerSession()
{
}

void* ServerSession::onMessage(std::string message)
{
  std::cout << "Handling message on Server side" << std::endl;
  std::cout << "Message received: " << message << std::endl;
  return nullptr;
}