#include "ClientSession.h"

#include <iostream>
#include <string>
#include <cstring>

#include "DirectoryManager.h"
#include "Utilities.h"

ClientSession::ClientSession(std::string host, int port, std::string username) :
Session(host, port),
username(username)
{
  std::cout << "Initializing Client-side session" << std::endl;

  std::cout << "Initializing Directory Manager" << std::endl;
  this->directoryManager = new DirectoryManager(username, 
                                                this);
}


ClientSession::~ClientSession()
{ 
  delete directoryManager;
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

    case Utilities::FILE:
      // message.content should have the file name
      std::cout << "Server requested file: " << message.content << std::endl;
      this->sendFile(message.content);
      break;

    default:
      std::cout << "Unknown field requested" << std::endl;
      break;
  }
}

void ClientSession::handleReply(Utilities::Message message)
{
  switch (message.field)
  {    
    case Utilities::BEGIN_OF_FILE:
      std::cout << "Recieving: " << std::string(message.content) << std::endl;
      break;
    
    case Utilities::CONTENT_OF_FILE:
      std::cout << "Content: " << std::string(message.content) << std::endl;
      break;

    case Utilities::END_OF_FILE:
      std::cout << "Finishing: " << std::string(message.content) << std::endl;
      break;

    default:
      std::cout << "Message with unknown field" << std::endl;
      break;
  }
}