#include "Server.h"

#include <iostream>

#include "Session.h"

Server::Server()
{
  std::cout << "Creating session" << std::endl;
  port = DEFAULT_PORT;
  Session *session = new Session("", port);

  Socket s = session->getSocket();
  s.bindSocket();
  s.listenSocket();
  s.acceptSocket();
}

Server::Server(int port) : 
port(port)
{
  std::cout << "Creating session" << std::endl;
  Session *session = new Session("", port);
  
  Socket s = session->getSocket();
  s.bindSocket();
  s.listenSocket();
  s.acceptSocket();
}

Server::~Server()
{

}

/* Synchronizes with the client's
* 'sync_dir_<username>' directory
*/
void Server::syncServer()
{

}

/* Receives a file sent by the client
*
*  file: file path
*/
void Server::receiveFile(std::string file)
{

}

/* Sends a file to the client
*
*  file: file path
*/
void Server::sendFile(std::string file)
{

}