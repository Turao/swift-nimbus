#include "ServerSession.h"

#include <iostream>
#include <cstring>

#include "Utilities.h"

ServerSession::ServerSession(Socket *s) :
Session(s)
{
  Utilities::Message requestMessage;
  std::string teste = "teste.txt";

  std::cout << "Initializing Server-side session" << std::endl;

  // to-do: remove

  requestMessage = {Utilities::REQUEST, Utilities::USERNAME};
  this->request(requestMessage);

  requestMessage = {Utilities::REQUEST, Utilities::FILE};
  strcpy(requestMessage.content, teste.c_str());
  this->request(requestMessage);
}


ServerSession::~ServerSession()
{
}

void* ServerSession::onMessage(Utilities::Message message)
{
  std::cout << "Handling message on Server side" << std::endl;
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

void ServerSession::handleRequest(Utilities::Message message)
{
}

void ServerSession::handleReply(Utilities::Message message)
{
	switch (message.field) {
		case Utilities::USERNAME:
			this->username = std::string(message.content);
			std::cout << "Username: " << this->username << std::endl;
			break;
    case Utilities::BEGIN_OF_FILE:
      std::cout << "Recieving: " << std::string(message.content) << std::endl;
      break;
    case Utilities::END_OF_FILE:
      std::cout << "Finishing: " << std::string(message.content) << std::endl;
      break;
    case Utilities::CONTENT_OF_FILE:
      std::cout << "Content: " << std::string(message.content) << std::endl;
      break;

	}
}