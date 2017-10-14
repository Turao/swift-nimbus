#include "Socket.h"

#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h> // close socket file descriptor
#include <errno.h>
#include <string.h>

Socket::Socket(std::string host, unsigned int port)
{
  std::cout << "Initializing socket" << std::endl;
  if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
  {
    std::cout << "Socket initialization failure: " 
              << strerror(errno)
              << std::endl;
    exit(1);
  }
  std::cout << "Socket succesfully initialized" << std::endl;


  // assigns local socket IP/PORT
  std::cout << "Assigning local host address" << std::endl;
  bzero(&s_local, sizeof(s_local));
  s_local.sin_family = AF_INET;
  s_local.sin_addr.s_addr = htonl(INADDR_ANY);
  s_local.sin_port = htons(port);

  // assigns remote socket IP/PORT
  if(!host.empty()) {
    std::cout << "Assigning remote host address" << std::endl;
    bzero(&s_remote, sizeof(s_remote));
    s_remote.sin_family = AF_INET;
    s_remote.sin_addr.s_addr = inet_addr(host.c_str());
    s_remote.sin_port = htons(port);
  }
}


Socket::Socket(int sockfd) : s(sockfd)
{
}



Socket::~Socket()
{
  close(s);
}


bool Socket::bindSocket()
{
  std::cout << "Binding socket data" << std::endl;
  // binds local information with socket
  if ((bind(s, (struct sockaddr *)&s_local, sizeof(s_local))) != 0)
  {
    std::cout << "Socket bind failure: " 
              << strerror(errno)
              << std::endl;
    shutdown(s, STOP_RECEPTION_TRANSMISSION);
    return false;
  }
  std::cout << "Socket bind successful" << std::endl;

  return true;
}


bool Socket::connectSocket()
{
  // establishes connection between local socket and remote socket
  std::cout << "Establishing connection" << std::endl;
  if(connect(s, (struct sockaddr*)&s_remote, sizeof(s_remote)) != 0)
  {
    std::cout << "Connection error: " 
              << strerror(errno)
              << std::endl;
    shutdown(s, STOP_RECEPTION_TRANSMISSION);
    return false;
  }
  std::cout << "Connection established" << std::endl;

  return true;
}


bool Socket::listenSocket()
{
  std::cout << "Listening on socket for incoming connections" << std::endl;
  if(listen(s, 8) < 0) {
    // allows up to 8 connections at the same time
    std::cout << "Error while listening on socket: "
              << strerror(errno)
              << std::endl;
    return false;
  }

  return true;
}


Socket* Socket::acceptSocket()
{
  std::cout << "Accepting connection" << std::endl;
  socklen_t s_local_len = sizeof(s_local);
  int sockfd;
  if((sockfd = accept(s, (struct sockaddr *) &s_local, 
                     &s_local_len)) < 0) {
    std::cout << "Error while accepting connection: " 
              << strerror(errno)
              << std::endl;
    return 0;
  }

  std::cout << "Creating accepted socket" << std::endl;
  Socket *accepted_socket = new Socket(sockfd);
  std::cout << "Accepted socket created" << std::endl;

  return accepted_socket;
}


int Socket::read(char* buffer, size_t length)
{
  return ::read(s, buffer, length);
}


void Socket::write(char* data, size_t length)
{
  ::write(s, data, length);
}