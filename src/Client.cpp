#include "Client.h"

#include <iostream>
#include <string>

#include "User.h"

Client::Client(std::string username)
{
  user = new User(username);
  std::cout << "user created" << std::endl;
  std::cout << user->getName() << std::endl;
}

Client::~Client()
{
  delete user;
}

/* Connects the client to the server
*  host: server address
*  port: server port
*/
int Client::connectServer(std::string host, int port)
{

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

}