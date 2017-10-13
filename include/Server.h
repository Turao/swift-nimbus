#pragma once

#include <string>

class Server
{
public:
  Server();
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
};
