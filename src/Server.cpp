#include "Server.h"

#include <iostream>

#include "Session.h"


Server::Server() :
port(DEFAULT_PORT),
master("", port)
{
  std::cout << "Creating session" << std::endl;

  master.bindSocket();
  master.listenSocket();
  Socket *connection0 = master.acceptSocket();


  Session *session = new Session(connection0);
}



Server::Server(int port) :
port(port),
master("", port)
{
  std::cout << "Creating session" << std::endl;

  master.bindSocket();
  master.listenSocket();
  Socket *connection0 = master.acceptSocket();


  Session *session = new Session(connection0);
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