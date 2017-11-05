#include "ServerSession.h"

#include <iostream>
#include <cstring>
#include <vector>

#include "DirectoryManager.h"
#include "NimbusFile.h"
#include "Utilities.h"
#include <time.h>

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


  // testing!!  

  std::vector<NimbusFile*> files = this->directoryManager->getFiles();
  for(auto it = files.begin(); it != files.end(); ++it) {
    std::string filepath = (*it)->getFilePath();
    std::cout << "Requesting file: " << filepath 
              << " to client" << std::endl;
    requestMessage = {Utilities::REQUEST, Utilities::FILE};
    strcpy(requestMessage.content, filepath.c_str());
    this->request(requestMessage);
  }

  std::string teste = "teste.txt";
  requestMessage = {Utilities::REQUEST, Utilities::FILE};
  strcpy(requestMessage.content, teste.c_str());
  Utilities::Message *replyMessage = (Utilities::Message *) this->request(requestMessage); 
  this->handleReply(*replyMessage);
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
      std::cout << "Recieving: " << std::string(message.content) << std::endl;
      std::cout << "Filesize: " << message.fileSize << std::endl;
      std::cout << "Lastmodified: " << asctime(localtime(&message.lastModified)) << std::endl;
      this->file = std::vector<char>();
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