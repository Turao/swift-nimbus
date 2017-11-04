#include "ClientSession.h"

#include <iostream>
#include <string>
#include <cstring>

#include "Utilities.h"

ClientSession::ClientSession(std::string host, int port, std::string username) :
Session(host, port),
username(username)
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

  switch(message.type)
  {
    case Utilities::REQUEST:
      this->handleRequest(message);
      break;

    case Utilities::REPLY:
      this->handleReply(message);
      break;
  }

  return nullptr;
}

void ClientSession::handleRequest(Utilities::Message message)
{
  std::cout << "Handling request" << std::endl;

  char replyContent[FILE_BLOCK_SIZE];
  Utilities::Message replyMessage;

  switch (message.field)
  {
    case Utilities::USERNAME:
      replyMessage = {Utilities::REPLY, Utilities::USERNAME};
      strncpy(replyMessage.content, this->username.c_str(), FILE_BLOCK_SIZE);
      this->reply(replyMessage);
      break;

    case Utilities::FILE: // message.content should have the file name
      this->sendFile(message.content);
      break;

    default:
      std::cout << "Unknown field requested" << std::endl;
      break;
  }
}

void ClientSession::handleReply(Utilities::Message message)
{
}