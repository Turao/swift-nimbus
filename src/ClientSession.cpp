#include "ClientSession.h"

#include <iostream>
#include <string>

ClientSession::ClientSession(std::string host, int port) :
Session(host, port)
{
  std::cout << "Initializing Client-side session" << std::endl;
}


ClientSession::~ClientSession()
{ 
}

void* ClientSession::onMessage(std::string message)
{
  std::cout << "Handling message on Client side" << std::endl;
  std::cout << "Message received: " << message << std::endl;
  return nullptr;
}