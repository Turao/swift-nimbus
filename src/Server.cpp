#include "Server.h"
#include "Session.h"

Server::Server()
{
  Session *session = new Session("", 8080);
  session->getSocket().listenSocket();
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