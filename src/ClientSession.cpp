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

    case Utilities::DELETE_FILE:
      std::cout << "Request to delete file: " << message.content << std::endl;
      this->deleteFile(message.content);
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

    case Utilities::LIST_FILES:
      std::cout << "File on server: " << std::string(message.content) << std::endl;
      std::cout << "Last Modified: " << asctime(localtime(&message.lastModified)) << std::endl;
      std::cout << "Status Change: " << asctime(localtime(&message.statusChange)) << std::endl;
      std::cout << "Last Access: " << asctime(localtime(&message.lastAccess)) << std::endl;
      break;
  }
}

void ClientSession::initializeDirectoryManager()
{
  this->directoryManager = new DirectoryManager(this->username, 
                                                this);
}

void ClientSession::printLocalDirectoryEntries()
{
  this->directoryManager->printDirectoryEntries();
}

void ClientSession::requestServerDirectoryEntries()
{
  Utilities::Message message = { Utilities::REQUEST,
                                 Utilities::LIST_FILES };
  Utilities::Message *response = static_cast<Utilities::Message*>( request(message) );
  handleReply(*response);
  delete response;
}

bool ClientSession::requestPermissionToSendFile(std::string file)
{
  bool isAllowed;
  
  Utilities::Message requestMessage;
  requestMessage = { Utilities::REQUEST, Utilities::FILE_TOKEN, file.size() };
  file.copy(requestMessage.content, file.size());

  std::cout << "Requesting token for: " << requestMessage.content << std::endl;

  Utilities::Message *response = static_cast<Utilities::Message*>( this->request(requestMessage) );
  isAllowed = response->field == Utilities::PERMISSION_GRANTED;
  std::cout << "Received answer for token: " << response->field << std::endl;
  delete response;

  return isAllowed;
}