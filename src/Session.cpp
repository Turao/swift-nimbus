#include "Session.h"

#include <thread>

Session::Session(std::string host, int port) :
teste(host, port)
{
  
}


Session::~Session()
{

}

Socket Session::getSocket()
{
  return this->teste; //TO-DO: UPDATE
}