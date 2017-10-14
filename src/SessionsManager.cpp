#include "SessionsManager.h"

#include <iostream>
#include <algorithm> // std::remove_if

SessionsManager::SessionsManager(Socket *master) :
master(master)
{
  this->scrubberThread = std::thread(&SessionsManager::scrubber, this);
  this->connectionsHandlerThread = std::thread(&SessionsManager::connectionsHandler, this);
}


SessionsManager::~SessionsManager()
{
  
  if(this->_scrubber_isRunning) {
    std::cout << "Stopping sessions manager threads (scrubber)" 
              << std::endl;
    stopScrubber();
  }
   
  if(this->_connectionsHandler_isRunning) {
    std::cout << "Stopping sessions manager threads (connections handler)" 
              << std::endl;
    stopConnectionsHandler();
  } 
}



void SessionsManager::stopScrubber()
{
  this->_scrubber_isRunning = false;
  this->scrubberThread.join();
}


void SessionsManager::scrubber()
{
  std::cout << "Initializing sessions scrubber thread" << std::endl;
  this->_scrubber_isRunning = true;
  while(_scrubber_isRunning)
  {
    std::cout << "Number of sessions: " << sessions.size() << std::endl;
    for(auto it = sessions.begin(); it != sessions.end(); ++it)
    {
      if(!(*it)->isAlive()) {
        // WARNING - THIS IS BREAKING THE CODE!
        std::cout << "Deleting dead session" << std::endl;
        // delete *it;
        std::cout << "Dead session deleted. Removing from vector" << std::endl;
        sessions.erase(it);
        std::cout << "Dead session removed from vector" << std::endl;
      }
    }
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }
}



void SessionsManager::stopConnectionsHandler()
{
  this->_connectionsHandler_isRunning = false;
  this->connectionsHandlerThread.join();
}


void SessionsManager::connectionsHandler()
{
  std::cout << "Initializing connections handler" << std::endl;
  this->_connectionsHandler_isRunning = true;
  while(_connectionsHandler_isRunning)
  {
    Socket *response = master->acceptSocket();
    if(response != nullptr) {
      std::cout << "New connection on master socket" << std::endl;
      Session *session = new Session(response);
      this->sessions.push_back(session);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}