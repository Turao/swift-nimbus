#include "Socket.h"

#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>


Socket::Socket(std::string host, int port)
{
  std::cout << "Initializing socket" << std::endl;
  if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
  {
    std::cout << "Socket initialization failure" << std::endl;
    exit(1);
  }
  std::cout << "Socket succesfully initialized" << std::endl;


  // assigns local socket IP/PORT
  s_local.sin_family = AF_INET;
  s_local.sin_addr.s_addr = htonl(INADDR_ANY);
  s_local.sin_port = htons(port);



  std::cout << "Binding socket data" << std::endl;
  // binds local information with socket
  if ((bind(s, (struct sockaddr *)&s_local, sizeof(s_local))) != 0)
  {
    std::cout << "Socket bind failure" << std::endl;
    shutdown(s, STOP_RECEPTION_TRANSMISSION);
    exit(1);
  }
  std::cout << "Socket bind succesful" << std::endl;


  // assigns remote socket IP/PORT
  if(!host.empty()) {
    std::cout << "Assigning remote host address" << std::endl;
    s_remote.sin_family = AF_INET;
    s_remote.sin_addr.s_addr = inet_addr(host.c_str());
    s_remote.sin_port = htons(port);
  }



  // establishes connection between local socket and remote socket
  std::cout << "Establishing connection" << std::endl;
  if(connect(s, (struct sockaddr*)&s_remote, sizeof(s_remote)) != 0)
  {
    std::cout << "Connection error" << std::endl;
    shutdown(s, STOP_RECEPTION_TRANSMISSION);
    exit(1);
  }
  std::cout << "Connection established" << std::endl;
}


Socket::Socket(int sockfd) : s(s)
{

}



Socket::~Socket()
{
  shutdown(s, STOP_RECEPTION_TRANSMISSION);
}

void Socket::listenSocket()
{
  std::cout << "Listening on socket for incoming connections" << std::endl;
  if(listen(s, 8) != 0) {
    // allows up to 8 connections at the same time
    std::cout << "Error while listening on socket" << std::endl;
    exit(1);
  }

  acceptSocket();
}

void Socket::acceptSocket()
{
  std::cout << "Accepting connection" << std::endl;
  struct sockaddr_in addr_cli;
  socklen_t addr_cli_len = sizeof(addr_cli);
  int sockfd;
  if((sockfd = accept(s, (struct sockaddr*) &addr_cli, 
                     (socklen_t*) &addr_cli_len)) < 0) {
    std::cout << "Error while accepting connection" << std::endl;
    exit(1);
  }

  std::cout << "Creating accepted socket" << std::endl;
  Socket *accepted_socket = new Socket(sockfd);
  std::cout << "Accepted socket created" << std::endl;
}
