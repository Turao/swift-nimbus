#include "Client.h"

#include <iostream>
#include <string>

#include "User.h"
#include "Session.h"

#include "DirectoryManager.h"

Client::Client(std::string username) : 
user(username)
{
  std::cout << "Initializing Directory Manager" << std::endl;
  this->directoryManager = new DirectoryManager(user.getName());
}

Client::~Client()
{
  delete directoryManager;
}

/* Connects the client to the server
*  host: server address
*  port: server port
*/
int Client::connectServer(std::string host, int port)
{
  std::cout << "Creating session" << std::endl;
  session = new Session(host, port);
  session->request("username");
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

}

/* Downloads a file from the server
*
*  file: file path
*/
void Client::getFile(std::string file)
{

}

/* Deletes a file from the local directory
*  'sync_dir_<username>'
*
*  file: file path
*/
void Client::deleteFile(std::string file)
{

}

/* Closes the connection with the server
*/
void Client::closeConnection()
{
  std::cout << "Closing session" << std::endl;
  delete session;
}