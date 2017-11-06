#include "ServerSession.h"

#include <iostream>
#include <cstring>
#include <vector>

#include "DirectoryManager.h"
#include "NimbusFile.h"
#include "Utilities.h"

ServerSession::ServerSession(Socket *s) :
Session(s)
{
  std::cout << "Initializing Server-side session" << std::endl;

  // requests username
  Utilities::Message requestMessage;
  requestMessage = { Utilities::REQUEST, Utilities::USERNAME };
  Utilities::Message *response = static_cast<Utilities::Message*>( this->request(requestMessage) );
  handleReply(*response); // to-do: check if got the username!
  delete response;

  // initializes directory manager for user on server side
  this->directoryManager = new DirectoryManager(username + "_server", 
                                                this);
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
  std::cout << "Handling request" << std::endl;

  char replyContent[FILE_BLOCK_SIZE];
  Utilities::Message replyMessage;

  switch (message.field)
  {
    case Utilities::FILE:
      // message.content should have the file name
      std::cout << "Client requested file: " << message.content << std::endl;
      this->sendFile(message.content);
      break;

    default:
      std::cout << "Unknown field requested" << std::endl;
      break;
  }
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
      this->fileName = std::string(message.content);
      this->fileSize = message.fileSize;
      this->fileLastModified = message.lastModified;
      this->file = std::vector<char>();
      std::cout << "Recieving: " << this->fileName << std::endl;
      std::cout << "Filesize: " << this->fileSize << std::endl;
      std::cout << "Last Modified: " << asctime(localtime(&this->fileLastModified)) << std::endl;
      break;
    
    case Utilities::CONTENT_OF_FILE:
      std::cout << "Content: " << std::string(message.content) << std::endl;
      int contentPointer;
      for (contentPointer = 0; contentPointer < message.contentSize; contentPointer++) {
        this->file.push_back(message.content[contentPointer]);
      }

      break;

    case Utilities::END_OF_FILE:
      std::cout << "Finishing: " << std::string(message.content) << std::endl;
      this->saveFile();
      break;

    default:
      std::cout << "Message with unknown field" << std::endl;
      break;
	}
}