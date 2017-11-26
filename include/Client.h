#pragma once

#include <string>
#include <atomic>
#include <thread>

#include "User.h"
#include "ClientSession.h"


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
  void sendFile(std::string path);


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

  /* Lists all entries within the 
  *  local sync directory
  */
  void listClientDirectoryEntries();


  /* Lists all entries within the 
  *  remote's sync directory
  */
  void listServerDirectoryEntries();


protected:
private:
  User user;
  ClientSession *session;
  

  std::atomic<bool> _commandThread_isRunning{false};
  std::thread _commandThread;
  void commandThread();
  void startCommandThread();
  void stopCommandThread();
  
};