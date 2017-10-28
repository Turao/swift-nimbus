#pragma once

#include <vector>

#include "Session.h"

class SessionsManager
{
public:
  SessionsManager(Socket *master);
  ~SessionsManager();

  void startScrubber();
  void stopScrubber();
  void stopConnectionsHandler();

protected:
  
private:
  Socket *master;
  std::vector<Session*> sessions;

  std::atomic<bool> _scrubber_isRunning;
  std::thread scrubberThread;
  void scrubber();


  std::atomic<bool> _connectionsHandler_isRunning;
  std::thread connectionsHandlerThread;
  void connectionsHandler();


};