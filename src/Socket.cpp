#include "Socket.h"

#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h> // close socket file descriptor
#include <fcntl.h> // set socket descriptor as non-blocking
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

// setting 15 second timeout
struct timeval tv;
tv.tv_sec = 15;
tv.tv_usec = 0;  // Not init'ing this can cause strange errors
setsockopt(s, 
           SOL_SOCKET, 
           SO_RCVTIMEO, 
           reinterpret_cast<char*>(&tv),
           sizeof(struct timeval));
}


Socket::Socket(int sockfd) : s(sockfd)
{
}



Socket::~Socket()
{
  std::cout << "Closing socket (" << this << ")" << std::endl;
  close(s);
  std::cout << "Socket closed (" << this << ")" << std::endl;
}


void Socket::bind()
{
  std::cout << "Binding socket data" << std::endl;
  // binds local information with socket
  if ((::bind(s, (struct sockaddr *)&s_local, sizeof(s_local))) != 0)
  {
    std::cout << "Socket bind failure: " 
              << strerror(errno)
              << std::endl;
    close(s);
    exit(1);
  }
  std::cout << "Socket bind successful" << std::endl;
}


void Socket::connect()
{
  // establishes connection between local socket and remote socket
  std::cout << "Establishing connection" << std::endl;
  if(::connect(s, (struct sockaddr*)&s_remote, sizeof(s_remote)) != 0)
  {
    std::cout << "Connection error: " 
              << strerror(errno)
              << std::endl;
    close(s);
    exit(1);
  }
  std::cout << "Connection established" << std::endl;

  // std::cout << "Changing socket to nonblocking mode" << std::endl;
  // // set socket descriptor to non-blocking mode
  // int flags = fcntl(s, F_GETFL, 0);
  // fcntl(s, F_SETFL, flags | O_NONBLOCK);
}


void Socket::listen()
{
  std::cout << "Listening on socket for incoming connections" << std::endl;
  if(::listen(s, 8) < 0) {
    // allows up to 8 connections at the same time
    std::cout << "Error while listening on socket: "
              << strerror(errno)
              << std::endl;
    exit(1);
  }
}


Socket* Socket::accept()
{
  std::cout << "Accepting connection" << std::endl;
  socklen_t s_local_len = sizeof(s_local);
  int sockfd;
  if((sockfd = ::accept(s, (struct sockaddr *) &s_local, 
                     &s_local_len)) < 0) {
    if(errno != EAGAIN && errno != EWOULDBLOCK ) {
      // EAGAIN and EWOULDBLOCK errors:
      // The socket is marked nonblocking and
      // no connections are present to be accepted.
      std::cout << "Error while accepting connection: " 
                << strerror(errno)
                << std::endl;
    }
    return nullptr;
  }

  // // set socket descriptor to non-blocking mode
  // int flags = fcntl(sockfd, F_GETFL, 0);
  // fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

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