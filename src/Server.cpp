#include "Server.h"

#include <iostream>
#include <vector>

Server::Server() :
port(DEFAULT_PORT),
master("", port),
sessionsManager(&master)
{
  std::cout << "Creating session" << std::endl;
  master.bind();
  master.listen();
}



Server::Server(int port) :
port(port),
master("", port),
sessionsManager(&master)
{
  std::cout << "Creating session" << std::endl;
  master.bind();
  master.listen();
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