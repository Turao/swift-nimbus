#pragma once

#include "Session.h"

#include <string>

#include "Utilities.h"


class ClientSession : public Session
{
public:
  ClientSession(std::string host, int port, std::string username);
  ~ClientSession();

  void initializeDirectoryManager();
  void printLocalDirectoryEntries();
  void requestServerDirectoryEntries();

protected:
private:  
  std::string username;

  void* onMessage(Utilities::Message message) override;
  void handleRequest(Utilities::Message message) override;
  void handleReply(Utilities::Message message) override;
};