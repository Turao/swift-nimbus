#include "Socket.h"

#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>


Socket::Socket(std::string host, int port)
{
  struct sockaddr_in s_cli, s_serv;

  std::cout << "Initializing socket" << std::endl;
  if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
  {
    std::cout << "Socket initialization failure" << std::endl;
    exit(1);
  }
  std::cout << "Socket succesfully initialized" << std::endl;



  // seta informacoes IP/Porta locais
  s_cli.sin_family = AF_INET;
  s_cli.sin_addr.s_addr = htonl(INADDR_ANY);
  s_cli.sin_port = htons(port);



  std::cout << "Binding socket data" << std::endl;
  // associa configuracoes locais com socket
  if ((bind(s, (struct sockaddr *)&s_cli, sizeof(s_cli))) != 0)
  {
    std::cout << "Socket bind failure" << std::endl;
    shutdown(s, STOP_RECEPTION_TRANSMISSION);
    exit(1);
  }
  std::cout << "Socket bind succesful" << std::endl;



  // seta informacoes IP/Porta do servidor remoto
  s_serv.sin_family = AF_INET;
  s_serv.sin_addr.s_addr = inet_addr(host.c_str());
  s_serv.sin_port = htons(port);



  // connecta socket aberto no cliente com o servidor
  std::cout << "Establishing connection" << std::endl;
  if(connect(s, (struct sockaddr*)&s_serv, sizeof(s_serv)) != 0)
  {
    std::cout << "Connection error" << std::endl;
    shutdown(s, STOP_RECEPTION_TRANSMISSION);
    exit(1);
  }
  std::cout << "Connection established" << std::endl;


}

Socket::~Socket()
{
  shutdown(s, STOP_RECEPTION_TRANSMISSION);
}