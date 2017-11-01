#include "ClientSession.h"

#include <iostream>
#include <string>

#include "Utilities.h"

ClientSession::ClientSession(std::string host, int port) :
Session(host, port)
{
  std::cout << "Initializing Client-side session" << std::endl;
}


ClientSession::~ClientSession()
{ 
}

void* ClientSession::onMessage(Utilities::Message message)
{
  std::cout << "Handling message on Client side" << std::endl;
  std::cout << "Message type: " << message.type << std::endl;
  std::cout << "Message field: " << message.field << std::endl;


  return nullptr;
}