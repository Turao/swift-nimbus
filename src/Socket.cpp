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

#include <openssl/ssl.h> // Open SSL
#include <openssl/err.h> // Open SSL errors

#define CERTIFICATE_FILEPATH "../CertFile.pem"
#define PRIVATE_KEY_FILEPATH "../WingedKey.pem"

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
Socket::Socket(std::string host, unsigned int port) :
ssl(nullptr),
openSSL_ctx(nullptr)
{
  init_socket();
  assign_local_host(port);
  
  if (host.empty()) {
    // Server-side socket (master)
    // don't initialize ssl layer here!
    // server-side ssl layer will be initialized for
    // each new socket accepted (opened)
  }
  else {
    // Client-side socket
    assign_remote_host(host, port);
    SSL_initClient();
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
s(sockfd),
ssl(nullptr),
openSSL_ctx(nullptr)
{
  // this is the socket opened on every accept call
  // so, it's a Server-side socket

  // all socket related attributes are already
  // initialized by its 'parent' socket
  
  SSL_initServer();
  SSL_bind();
  // accepts the SSL handshake started by the client
  SSL_accept();
}



Socket::~Socket()
{
  std::cout << "Closing socket (" << this << ")" << std::endl;
  
  close(s);
  SSL_cleanup();

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
    SSL_CTX_free(openSSL_ctx);
    exit(1);
  }

  // binds the SSL structure to the socket
  SSL_bind();
  
  // connects on the SSL layer
  // (starts the SSL handshake)
  SSL_connect();

  std::cout << "Connection established" << std::endl;

  SSL_showCertificate();
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
  return SSL_read(ssl, buffer, length);
}


void Socket::write(char* data, size_t length)
{
  // // Normal socket write function
  // ::write(s, data, length);

  // SSL layer write
  SSL_write(ssl, data, length);
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



/*----------  OPEN SSL FUNCTIONS -----------*/

// In case both sides decide to use certificates:
// (just delete the specific client/server functions 
//  and call this one)
//
// void Socket::SSL_init()
// {
//   // initializes open ssl library
//   // loads algorithms and error strings for debugging
//   _SSL_base_init();

//   // sets SSLv23 as the connection method
//   _SSL_create_context(TLSv1_method());

//   _SSL_load_certificate();

//   // creates ssl layer structure
//   ssl = SSL_new(openSSL_ctx);
// }



void Socket::SSL_initClient()
{
  std::cout << "Client side ssl" << std::endl;

  // initializes open ssl library
  // loads algorithms and error strings for debugging
  _SSL_base_init();

  // sets SSLv23 as the connection method
  _SSL_create_context(TLSv1_client_method());

  // creates ssl layer structure
  ssl = SSL_new(openSSL_ctx);

}


void Socket::SSL_initServer()
{
  std::cout << "Server side ssl" << std::endl;

  // initializes open ssl library
  // loads algorithms and error strings for debugging
  _SSL_base_init();

  // sets SSLv23 as the connection method
  _SSL_create_context(TLSv1_server_method());

  _SSL_load_certificate();

  // creates ssl layer structure
  ssl = SSL_new(openSSL_ctx);  
}



void Socket::_SSL_base_init()
{
  // "If you fail to initialize the library, then you will 
  //  experience unexplained errors like 
  //  SSL_CTX_new returning NULL, 
  //  error messages like SSL_CTX_new:library has no ciphers 
  //  and alert handshake failure with no shared ciphers."


  // loads the open ssl
  SSL_library_init();

  // adds ciphers and digests
  // (loads libcrypto)
  OpenSSL_add_all_algorithms();
  OpenSSL_add_ssl_algorithms();
  SSL_load_error_strings();
}


void Socket::_SSL_create_context(const SSL_METHOD* method)
{
  if (method == nullptr) {
    std::cout << "Invalid connection method"
              << std::endl;
    SSL_cleanup();
    exit(1);
  }

  openSSL_ctx = SSL_CTX_new(method);
  if (openSSL_ctx  ==  nullptr) {
    std::cout << "Error while initializing SSL context:"
              << std::endl;
    SSL_cleanup();

    ERR_print_errors_fp(stderr);
    exit(1);
  }
}


void Socket::SSL_bind()
{
  SSL_set_fd(ssl, s);
}



void Socket::_SSL_load_certificate()
{
  if (SSL_CTX_use_certificate_file(openSSL_ctx, 
                                   CERTIFICATE_FILEPATH,
                                   SSL_FILETYPE_PEM) <= 0) {
    std::cout << "Error while setting the certificate file"
              << std::endl;
    SSL_cleanup();
    exit(1);
  }

  // adds private key file into SSL context
  if (SSL_CTX_use_PrivateKey_file(openSSL_ctx, 
                                  PRIVATE_KEY_FILEPATH,
                                  SSL_FILETYPE_PEM) <= 0) {
    std::cout << "Error while setting the key file"
              << std::endl;
    SSL_cleanup();
    exit(1);
  }

  // makes sure the key and certificate file match
  if (SSL_CTX_check_private_key(openSSL_ctx) == 0) {
    std::cout << "Private key does not match the certificate public key"
              << std::endl;
    SSL_cleanup();
    exit(1);
  }
}


void Socket::SSL_showCertificate()
{
  X509 *cert = SSL_get_peer_certificate(ssl);

  if (cert !=  nullptr) {
    std::cout << "Subject:" << std::endl;
    X509_NAME_print_ex_fp(stderr,
                          X509_get_subject_name(cert),
                          2,
                          XN_FLAG_MULTILINE);    

    std::cout << std::endl;
    
    std::cout << "Issuer:" << std::endl;
    X509_NAME_print_ex_fp(stderr,
                          X509_get_issuer_name(cert),
                          2, XN_FLAG_MULTILINE);

    std::cout << std::endl;
  }

  X509_free(cert);
}


void Socket::SSL_accept()
{
  // Do the SSL handshake
  if (::SSL_accept(ssl) < 1) {
    std::cout << "Error during SSL handshake (accept):"
              << std::endl;

    ERR_print_errors_fp(stderr);

    _SSL_list_ciphers();

    close(s);
    SSL_cleanup();
    exit(1);
  }
}

void Socket::SSL_connect()
{
  // connects to the server, SSL layer
  if (::SSL_connect(ssl) != 1) {
    std::cout << "SSL Connection error:"
              << std::endl;
    ERR_print_errors_fp(stderr);

    _SSL_list_ciphers();

    close(s);
    SSL_cleanup();
    exit(1);
  }
}


void Socket::SSL_cleanup()
{
  if (ssl) SSL_free(ssl);
  if (openSSL_ctx) SSL_CTX_free(openSSL_ctx);
  // EVP_cleanup(); // cleanup openssl
}


void Socket::_SSL_list_ciphers()
{
  int priority = 0;
  while (auto cipher = SSL_get_cipher_list(ssl, priority))
  {
    std::cout << cipher << std::endl;
    priority++;
  }
}