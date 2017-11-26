#pragma once

#include <vector>
#include <mutex>

#include "ServerSession.h"

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
  std::vector<ServerSession*> sessions;

  std::mutex sessions_mtx;

  std::atomic<bool> _scrubber_isRunning;
  std::thread scrubberThread;
  void scrubber();


  std::atomic<bool> _connectionsHandler_isRunning;
  std::thread connectionsHandlerThread;
  void connectionsHandler();


};