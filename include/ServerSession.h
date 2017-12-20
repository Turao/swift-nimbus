#pragma once

#include "Session.h"

#include <string>
#include <vector>

#include "Socket.h"
#include "Utilities.h"


class ServerSession : public Session
{
public:
  ServerSession(Socket *s, std::vector<std::string> &tl);
  ~ServerSession();

  bool requestPermissionToSendFile(std::string file) override;

protected:
private:
  std::string username;
  std::mutex token_mtx;
  std::vector<std::string> *tokenlist;

  void* onMessage(Utilities::Message message) override;
  void handleReply(Utilities::Message message) override;
  void handleRequest(Utilities::Message message) override;

  void addFileToken(std::string file);
  void removeFileToken(std::string file);
  bool getFileToken(std::string file);
  
  void sendFilesList();
  
};