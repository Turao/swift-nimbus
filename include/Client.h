#pragma once

#include <string>

#include "User.h"

class Client
{
public:
  Client(std::string username);
  ~Client();
  
  /* Connects the client to the server
  *  host: server address
  *  port: server port
  */
  int connectServer(const std::string host, int port);


  /* Synchronizes 'sync_dir_<username>' directory
  *  with the server
  */
  void syncClient();


  /* Uploads a local file to the server
  *
  *  file: file path
  */
  void sendFile(std::string file);


  /* Downloads a file from the server
  *
  *  file: file path
  */
  void getFile(std::string file);


  /* Deletes a file from the local directory
  *  'sync_dir_<username>'
  *
  *  file: file path
  */
  void deleteFile(std::string file);


  /* Closes the connection with the server
  */
  void closeConnection();


protected:
private:
  User *user;

};