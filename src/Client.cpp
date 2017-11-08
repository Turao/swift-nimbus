#include "Client.h"

#include <iostream>
#include <string>
#include <cstring>
#include <thread>

#include "User.h"
#include "Session.h"
#include "DirectoryManager.h"

#include "Utilities.h"


Client::Client(std::string username) : 
user(username)
{
  std::cout << "Initializing command thread" << std::endl;
  startCommandThread();
  
}

Client::~Client()
{
  stopCommandThread();
}

/* Connects the client to the server
*  host: server address
*  port: server port
*/
int Client::connectServer(std::string host, int port)
{
  std::cout << "Creating session" << std::endl;
  session = new ClientSession(host, port, this->user.getName());
  
  return 0; // to-do: return something
}

/* Synchronizes 'sync_dir_<username>' directory
*  with the server
*/
void Client::syncClient()
{
  this->session->initializeDirectoryManager();
}

/* Uploads a local file to the server
*
*  file: file path
*/
void Client::sendFile(std::string path)
{
  std::cout << "Uploading file: " << path << std::endl;

  this->session->sendFile(path.c_str());
}

/* Downloads a file from the server
*
*  file: file path
*/
void Client::getFile(std::string file)
{
  std::cout << "Downloading file: " << file << std::endl;
  Utilities::Message request = { Utilities::REQUEST,
                                 Utilities::FILE };
  strncpy(request.content, file.c_str(), FILE_BLOCK_SIZE);

  this->session->downloadToCurrDir = true;
  this->session->request(request);
}

/* Deletes a file from the local directory
*  'sync_dir_<username>'
*
*  file: file path
*/
void Client::deleteFile(std::string file)
{
  std::cout << "Deleting file: " << file << std::endl;
  this->session->deleteFile(file);
}

/* Closes the connection with the server
*/
void Client::closeConnection()
{
  std::cout << "Closing session" << std::endl;
  delete session;
}

void Client::listClientDir()
{
  this->session->printLocalDir();
}

void Client::listServerDir()
{
  Utilities::Message request = { Utilities::REQUEST,
                                 Utilities::LIST_FILES };
  Utilities::Message *response = static_cast<Utilities::Message*>( this->session->request(request) );
  this->session->handleReply(*response);
  delete response;
}


void Client::startCommandThread()
{
  this->_commandThread = std::thread(&Client::commandThread, this);
}

void Client::stopCommandThread()
{
  this->_commandThread_isRunning = false;
  this->_commandThread.join();
}

void Client::commandThread()
{
  _commandThread_isRunning = true;
  std::string input, command, argument;
  while(_commandThread_isRunning)
  {
    std::getline (std::cin, input);
    int pos = input.find_first_of(" ");
    command = input.substr(0, pos);
    argument = input.substr(pos+1);

    std::cout << "Command: " << command
              << "Argument: " << argument
              << std::endl;
    
    if(command.compare("upload") == 0)
      sendFile(argument);
    else if(command.compare("download") == 0)
      getFile(argument);
    else if(command.compare("deleteFile") == 0)
      deleteFile(argument);
    else if(command.compare("get_sync_dir") == 0)
      syncClient();
    else if(command.compare("exit") == 0)
      closeConnection();
    else if(command.compare("list_server") == 0)
      listServerDir();
    else if(command.compare("list_client") == 0)
      listClientDir();
    else
      std::cout << "Unknown command" << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}