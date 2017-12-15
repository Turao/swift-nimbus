#include "SecureSocketLayer.h"

#include <iostream>

#include <openssl/ssl.h> // Open SSL
#include <openssl/err.h> // Open SSL errors


/*----------  OPEN SSL FUNCTIONS -----------*/

// In case both sides decide to use certificates:
// (just delete the specific client/server functions 
//  and call this one)
//
// void SecureSocketLayer::init(SOCKET sockfd)
// {
//   // initializes open ssl library
//   // loads algorithms and error strings for debugging
//   base_init();

//   // sets SSLv23 as the connection method
//   create_context(TLSv1_method());

//   load_certificate();

//   // creates ssl layer structure
//   ssl = SSL_new(openSSL_ctx);

//   // binds the underlying socket to the secure layer
//   bind(sockfd);
// }

SecureSocketLayer::SecureSocketLayer()
{
}


SecureSocketLayer::~SecureSocketLayer()
{
  cleanup();
}


void SecureSocketLayer::initClient(SOCKET sockfd)
{
  std::cout << "Client side ssl" << std::endl;

  // initializes open ssl library
  // loads algorithms and error strings for debugging
  // and assigns the underlying socket
  base_init();

  // sets SSLv23 as the connection method
  create_context(TLSv1_client_method());

  // creates ssl layer structure
  ssl = SSL_new(openSSL_ctx);

  // binds the underlying socket to the secure layer
  bind(sockfd);
}


void SecureSocketLayer::initServer(SOCKET sockfd)
{
  std::cout << "Server side ssl" << std::endl;

  // initializes open ssl library
  // loads algorithms and error strings for debugging
  // and assigns the underlying socket
  base_init();

  // sets SSLv23 as the connection method
  create_context(TLSv1_server_method());

  load_certificate();

  // creates ssl layer structure
  ssl = SSL_new(openSSL_ctx);

  // binds the underlying socket to the secure layer
  bind(sockfd);
}



void SecureSocketLayer::base_init()
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


void SecureSocketLayer::create_context(const SSL_METHOD* method)
{
  if (method == nullptr) {
    std::cout << "Invalid connection method"
              << std::endl;
    cleanup();
    exit(1);
  }

  openSSL_ctx = SSL_CTX_new(method);
  if (openSSL_ctx  ==  nullptr) {
    std::cout << "Error while initializing SSL context:"
              << std::endl;
    ERR_print_errors_fp(stderr);
    cleanup();
    exit(1);
  }
}


void SecureSocketLayer::bind(SOCKET sockfd)
{
  SSL_set_fd(ssl, sockfd);
}



void SecureSocketLayer::load_certificate()
{
  if (SSL_CTX_use_certificate_file(openSSL_ctx, 
                                   CERTIFICATE_FILEPATH,
                                   SSL_FILETYPE_PEM) <= 0) {
    std::cout << "Error while setting the certificate file"
              << std::endl;
    cleanup();
    exit(1);
  }

  // adds private key file into SSL context
  if (SSL_CTX_use_PrivateKey_file(openSSL_ctx, 
                                  PRIVATE_KEY_FILEPATH,
                                  SSL_FILETYPE_PEM) <= 0) {
    std::cout << "Error while setting the key file"
              << std::endl;
    cleanup();
    exit(1);
  }

  // makes sure the key and certificate file match
  if (SSL_CTX_check_private_key(openSSL_ctx) == 0) {
    std::cout << "Private key does not match the certificate public key"
              << std::endl;
    cleanup();
    exit(1);
  }
}


void SecureSocketLayer::showCertificate()
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


void SecureSocketLayer::accept()
{
  // Do the SSL handshake
  if (SSL_accept(ssl) < 1) {
    std::cout << "Error during SSL handshake (accept):"
              << std::endl;

    ERR_print_errors_fp(stderr);

    list_ciphers();

    cleanup();
    exit(1);
  }
}

void SecureSocketLayer::connect()
{
  // connects to the server, SSL layer
  if (SSL_connect(ssl) != 1) {
    std::cout << "SSL Connection error:"
              << std::endl;
    ERR_print_errors_fp(stderr);

    list_ciphers();

    cleanup();
    exit(1);
  }
}


void SecureSocketLayer::cleanup()
{
  if (ssl) SSL_free(ssl);
  if (openSSL_ctx) SSL_CTX_free(openSSL_ctx);
  EVP_cleanup(); // cleanup openssl
}


int SecureSocketLayer::read(char* buffer, size_t length)
{
  return SSL_read(ssl, buffer, length);
}


void SecureSocketLayer::write(char* data, size_t length)
{
  SSL_write(ssl, data, length);
}


void SecureSocketLayer::list_ciphers()
{
  int priority = 0;
  while (auto cipher = SSL_get_cipher_list(ssl, priority))
  {
    std::cout << cipher << std::endl;
    priority++;
  }
}