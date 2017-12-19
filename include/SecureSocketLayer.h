#pragma once

#include <openssl/ssl.h> // Open SSL

#define SOCKET int

#define CERTIFICATE_FILEPATH "../CertFile.pem"
#define PRIVATE_KEY_FILEPATH "../WingedKey.pem"


class SecureSocketLayer
{
public:
  SecureSocketLayer();
  ~SecureSocketLayer();

  void initClient(SOCKET sockfd);
  void initServer(SOCKET sockfd);

  void showCertificate();
  
  void accept();
  void connect();

  int read(char* buffer, size_t length);
  void write(char* data, size_t length);

  void cleanup();

protected:
private:
  SSL *ssl;
  SSL_CTX *openSSL_ctx;
  
  // void SSL_init();  
  void base_init();
  void bind(SOCKET sockfd);
  void create_context(const SSL_METHOD* method);
  void list_ciphers();
  void load_certificate();
};

  