#include "ClientSession.h"

#include <iostream>
#include <string>
#include <cstring>

#include "Utilities.h"

ClientSession::ClientSession(std::string host, int port, std::string username) :
Session(host, port)
{
  std::cout << "Initializing Client-side session" << std::endl;

  this->username = username;
}


ClientSession::~ClientSession()
{ 
}

void* ClientSession::onMessage(Utilities::Message message)
{
  std::cout << "Handling message on Client side" << std::endl;
  std::cout << "Message type: " << message.type << std::endl;
  std::cout << "Message field: " << message.field << std::endl;

  switch(message.type) {
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
	char *replyContent = new char[FILE_BLOCK_SIZE];
	Utilities::Message replyMessage;

  std::cout << "Handling request" << std::endl;

	switch (message.field) {
		case Utilities::USERNAME:
			replyMessage = {Utilities::REPLY, Utilities::USERNAME};
      strcpy(replyMessage.content, this->username.c_str());
			this->reply(replyMessage);
			break;
		case Utilities::FILE:		// message.content should have the file name
			this->sendFile(message.content);
      break;

	}

	delete[] replyContent;
}

void ClientSession::handleReply(Utilities::Message message)
{

}