#pragma once

#include <string>
#include <netinet/in.h>

#define SOCKET int
#define INVALID_SOCKET  ((SOCKET)~0)

#define STOP_RECEPTION 0
#define STOP_TRANSMISSION 1
#define STOP_RECEPTION_TRANSMISSION 2

class Socket
{
public:
  Socket(std::string host, int port);
  Socket(int sockfd);
  ~Socket();

  bool bindSocket();
  bool connectSocket();
  bool listenSocket();
  bool acceptSocket();

protected:
private:
  SOCKET s;
  struct sockaddr_in s_local; // local socket info
  struct sockaddr_in s_remote; // remote socket info
};