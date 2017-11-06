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

}

/* Uploads a local file to the server
*
*  file: file path
*/
void Client::sendFile(std::string file)
{
  std::cout << "Uploading file: " << file << std::endl;
  this->session->sendFile(file.c_str());
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
  std::string command, argument;
  while(_commandThread_isRunning)
  {
    std::cin >> command >> argument;
    
    if(command.compare("sendFile") == 0)
      sendFile(argument);
    else if(command.compare("getFile") == 0)
      getFile(argument);
    else if(command.compare("deleteFile") == 0)
      deleteFile(argument);
    else
      std::cout << "Unknown command" << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}