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


#define SOCKET_TIMEOUT 15

/* Client-side socket
*   Needs to specify:
* - destination host
* - destination port
*
*  Server-side socket
*   Needs to specify:
* - source port
*/
Socket::Socket(std::string host, unsigned int port)
{
  init_socket();
  assign_local_host(port);
  
  if (host.empty()) {
    // Server-side socket (master)
    // don't initialize ssl layer here!
    // server-side ssl layer will be initialized for
    // each new socket accepted (opened)
  
    reuse_addr(true);
    reuse_port(true);
  }
  else {
    // Client-side socket
    assign_remote_host(host, port);
    ssl.initClient(s);
  }
  
  set_timeout(SOCKET_TIMEOUT);
}



/* Server-side socket
*   Opened by the master socket when accepting 
*   incoming connections. 
*   Needs to specify:
* - socked file descriptor created by the 'parent' socket
*/
Socket::Socket(int sockfd) :
s(sockfd)
{
  // this is the socket opened on every accept call
  // so, it's a Server-side socket

  // all socket related attributes are already
  // initialized by its 'parent' socket
  // so, we only initialize the secure layer
  ssl.initServer(s);
  
  
  // accepts the SSL handshake started by the client
  ssl.accept();
}



Socket::~Socket()
{
  std::cout << "Closing socket (" << this << ")" << std::endl;
  
  close(s);
  ssl.cleanup();

  std::cout << "Socket closed (" << this << ")" << std::endl;
}


void Socket::bind()
{
  std::cout << "Binding socket data" << std::endl;
  // binds local information with socket
  if ((::bind(s, (struct sockaddr *)&s_local, sizeof(s_local))) != 0) {
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
  
  if (::connect(s, (struct sockaddr*)&s_remote, sizeof(s_remote)) != 0) {
    std::cout << "Connection error: " 
              << strerror(errno)
              << std::endl;

    close(s);
    ssl.cleanup();
    exit(1);
  }

  // connects on the SSL layer
  // (starts the SSL handshake)
  ssl.connect();

  std::cout << "Connection established" << std::endl;

  ssl.showCertificate();
}


void Socket::listen()
{
  std::cout << "Listening on socket for incoming connections" << std::endl;
  if (::listen(s, 8) < 0) {
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
  int client_socket;
  if ((client_socket = ::accept(s, (struct sockaddr *) &s_local, 
                         &s_local_len)) < 0) {
    if (errno != EAGAIN && errno != EWOULDBLOCK ) {
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
  // int flags = fcntl(client_socket, F_GETFL, 0);
  // fcntl(client_socket, F_SETFL, flags | O_NONBLOCK);

  std::cout << "Creating accepted socket" << std::endl;
  Socket *accepted_socket = new Socket(client_socket);
  std::cout << "Accepted socket created" << std::endl;

  return accepted_socket;
}


int Socket::read(char* buffer, size_t length)
{
  // // Normal socket read function
  // return ::read(s, buffer, length);

  // SSL layer read
  return ssl.read(buffer, length);
}


void Socket::write(char* data, size_t length)
{
  // // Normal socket write function
  // ::write(s, data, length);

  // SSL layer write
  ssl.write(data, length);
}


void Socket::init_socket()
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
}


void Socket::assign_local_host(unsigned int port)
{
  // assigns local socket IP/PORT
  std::cout << "Assigning local host address" << std::endl;
  bzero(&s_local, sizeof(s_local));
  s_local.sin_family = AF_INET;
  s_local.sin_addr.s_addr = htonl(INADDR_ANY);
  s_local.sin_port = htons(port);
}


void Socket::assign_remote_host(std::string host, unsigned int port)
{
  // assigns remote socket IP/PORT
  if (!host.empty()) {
    std::cout << "Assigning remote host address" << std::endl;
    bzero(&s_remote, sizeof(s_remote));
    s_remote.sin_family = AF_INET;
    s_remote.sin_addr.s_addr = inet_addr(host.c_str());
    s_remote.sin_port = htons(port);
  }
  else
  {
    std::cout << "Error while assigning remote host: "
              << "Host not specified" 
              << std::endl;
  }
}


void Socket::set_timeout(unsigned int timeout)
{
  // setting 15 second timeout
  struct timeval tv;
  tv.tv_sec = timeout;
  tv.tv_usec = 0;  // Not init'ing this can cause strange errors
  setsockopt(s, 
             SOL_SOCKET, 
             SO_RCVTIMEO, 
             reinterpret_cast<char*>(&tv),
             sizeof(struct timeval));
}



void Socket::reuse_port(bool reuse)
{
#ifdef SO_REUSEPORT
  if (setsockopt(s,
                 SOL_SOCKET,
                 SO_REUSEPORT,
                 (const char*) &reuse,
                 sizeof(reuse) ) < 0) 
  std::cout << "Error while setting SO_REUSEPORT option."
            << std::endl;
#endif
}


void Socket::reuse_addr(bool reuse)
{
  if (setsockopt(s,
                 SOL_SOCKET,
                 SO_REUSEADDR,
                 (const char*) &reuse,
                 sizeof(reuse) ) < 0) 
  std::cout << "Error while setting SO_REUSEADDR option."
            << std::endl;
}
