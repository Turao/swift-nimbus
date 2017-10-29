#include "Server.h"

#include <iostream>
#include <vector>

#include "Session.h"


#include "NimbusFile.h" //testing

Server::Server() :
port(DEFAULT_PORT),
master("", port),
sessionsManager(&master)
{
  std::cout << "Creating session" << std::endl;
  master.bind();
  master.listen();

  while(1)
  {
    std::this_thread::sleep_for (std::chrono::seconds(1));
  }
}



Server::Server(int port) :
port(port),
master("", port),
sessionsManager(&master)
{
  std::cout << "Creating session" << std::endl;
  master.bind();
  master.listen();


  this->file = new NimbusFile("/home/leonardo/Área de Trabalho/teste.txt");

  while(1)
  {
    std::this_thread::sleep_for (std::chrono::seconds(1));
  }

}

Server::~Server()
{
  delete this->file;
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