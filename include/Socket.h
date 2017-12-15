#pragma once

#include <string>
#include <netinet/in.h>

#include <openssl/ssl.h> // Open SSL
#include <openssl/err.h> // Open SSL errors

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
  
  void init_socket();
  void assign_local_host(unsigned int port);
  void assign_remote_host(std::string host, unsigned int port);
  void set_timeout(unsigned int timeout);


  /* Open SSL Layer methods */
  
  SSL *ssl;
  SSL_CTX *openSSL_ctx;
  
  void SSL_init();

  void SSL_initClient();
  void SSL_initServer();

  void SSL_bind();
  void SSL_showCertificate();
  void SSL_accept();
  void SSL_connect();
  void SSL_cleanup();
  
  void _SSL_base_init();
  void _SSL_create_context(const SSL_METHOD* method);
  void _SSL_list_ciphers();
  void _SSL_load_certificate();

};