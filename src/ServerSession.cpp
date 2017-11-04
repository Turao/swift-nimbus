#include "ServerSession.h"

#include <iostream>
#include <cstring>

#include "DirectoryManager.h"
#include "Utilities.h"

ServerSession::ServerSession(Socket *s) :
Session(s),
directoryManager(nullptr)
{
  std::cout << "Initializing Server-side session" << std::endl;

  // requests username
  Utilities::Message requestMessage;
  requestMessage = {Utilities::REQUEST, Utilities::USERNAME};
  Utilities::Message *response = static_cast<Utilities::Message*>( this->request(requestMessage) );
  handleReply(*response); // to-do: check if got the username!

  // initializes directory manager for user on server side
  this->directoryManager = new DirectoryManager(this->username);
}


ServerSession::~ServerSession()
{
}


void* ServerSession::onMessage(Utilities::Message message)
{
  std::cout << "Handling message on Server side" << std::endl;
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


void ServerSession::handleRequest(Utilities::Message message)
{
}


void ServerSession::handleReply(Utilities::Message message)
{
	switch (message.field)
  {
		case Utilities::USERNAME:
			this->username = std::string(message.content);
			std::cout << "Username: " << this->username << std::endl;
			break;
    
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