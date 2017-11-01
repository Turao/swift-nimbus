#include "ServerSession.h"

#include <iostream>
#include <cstring>

#include "Utilities.h"

ServerSession::ServerSession(Socket *s) :
Session(s)
{
  std::cout << "Initializing Server-side session" << std::endl;
}


ServerSession::~ServerSession()
{
}

void* ServerSession::onMessage(Utilities::Message message)
{
  std::cout << "Handling message on Server side" << std::endl;
  std::cout << "Message type: " << message.type << std::endl;
  std::cout << "Message field: " << message.field << std::endl;


  return nullptr;
}