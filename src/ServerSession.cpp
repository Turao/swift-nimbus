#include "ServerSession.h"

#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>

#include "DirectoryManager.h"
#include "NimbusFile.h"
#include "Utilities.h"

ServerSession::ServerSession(Socket *s, std::vector<std::string> &tl) :
Session(s)
{
  std::cout << "Initializing Server-side session" << std::endl;
  this->tokenlist = &tl;

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
  Utilities::Message reply;

  switch (message.field)
  {
    case Utilities::FILE:
      // message.content should have the file name
      std::cout << "Client requested file: " << message.content << std::endl;
      this->sendFile(message.content);
      break;

    case Utilities::DELETE_FILE:
      std::cout << "Request to delete file: " << message.content << std::endl;
      this->deleteFile(message.content);
      break;

    case Utilities::LIST_FILES:
      this->sendFilesList();
      break;

    case Utilities::FILE_TOKEN:
      token_mtx.lock();
      if (!this->getFileToken(message.content)) {
        this->addFileToken(message.content);
        reply = { Utilities::REPLY, Utilities::PERMISSION_GRANTED };
      }
      else
        reply = { Utilities::REPLY, Utilities::PERMISSION_DENIED };
      token_mtx.unlock();
      this->reply(reply);
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
      token_mtx.lock();
      this->removeFileToken(message.content);
      token_mtx.unlock();
      break;

    default:
      std::cout << "Message with unknown field" << std::endl;
      break;
	}
}

void ServerSession::sendFilesList()
{
  this->directoryManager->lockFiles();

  std::vector<NimbusFile*> dirFiles = this->directoryManager->getFiles();

  for(auto it = dirFiles.begin(); it != dirFiles.end(); ++it) {
    NimbusFile *file = *it;
    Utilities::Message reply = { Utilities::REPLY,
                                 Utilities::LIST_FILES };
    std::string fileName = file->getName() + "." + file->getExtension();
    strncpy(reply.content, fileName.c_str(), FILE_BLOCK_SIZE);
    reply.lastModified = file->getLastModified();
    reply.statusChange = file->getStatusChange();
    reply.lastAccess = file->getLastAccess();
    this->reply(reply);
  }

  this->directoryManager->unlockFiles();
}

bool ServerSession::requestPermissionToSendFile(std::string file)
{
  return true;
}

void ServerSession::addFileToken(std::string file)
{
  tokenlist->push_back(username + file);
}

void ServerSession::removeFileToken(std::string file)
{
  if (getFileToken(file))
    tokenlist->erase(std::find(tokenlist->begin(), tokenlist->end(), username + file));
}

bool ServerSession::getFileToken(std::string file)
{
  return std::find(tokenlist->begin(), tokenlist->end(), username + file) != tokenlist->end();
}