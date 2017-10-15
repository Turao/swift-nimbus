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
  Socket(std::string host, unsigned int port);
  Socket(int sockfd);
  ~Socket();

  void bind();
  void listen();
  Socket* accept();
  
  void connect();

  int read(char* buffer, size_t length);
  void write(char* data, size_t length);

protected:
private:
  SOCKET s;
  struct sockaddr_in s_local; // local socket info
  struct sockaddr_in s_remote; // remote socket info
};