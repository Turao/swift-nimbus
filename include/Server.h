#pragma once

#include <string>
#include <vector>

#include "Socket.h"
#include "Session.h"
#include "SessionsManager.h"
#include "NimbusFile.h"

#define DEFAULT_PORT 8080

class Server
{
public:
  Server();
  Server(int port);
  ~Server();

  
  /* Synchronizes with the client's
  * 'sync_dir_<username>' directory
  */
  void syncServer();


  /* Receives a file sent by the client
  *
  *  file: file path
  */
  void receiveFile(std::string file);


  /* Sends a file to the client
  *
  *  file: file path
  */
  void sendFile(std::string file);

protected:
private:
  int port;
  Socket master;
  SessionsManager sessionsManager;

  void initMasterSocket();
};
