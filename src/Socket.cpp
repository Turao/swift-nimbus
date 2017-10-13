#include "Socket.h"

#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#define SOCKET int
#define INVALID_SOCKET  ((SOCKET)~0)

#define STOP_RECEPTION 0
#define STOP_TRANSMISSION 1
#define STOP_RECEPTION_TRANSMISSION 2


Socket::Socket(std::string host, int port)
{

  struct sockaddr_in peer, s_cli, s_serv;
  SOCKET s;
  if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
  {
    printf("Erro iniciando socket\n");
    exit(1);
  }

  std::cout << "aqui0" << std::endl;


  // seta informacoes IP/Porta locais
  s_cli.sin_family = AF_INET;
  s_cli.sin_addr.s_addr = htonl(INADDR_ANY);
  s_cli.sin_port = htons(port);

  // associa configuracoes locais com socket
  if ((bind(s, (struct sockaddr *)&s_cli, sizeof(s_cli))) != 0)
  {
    printf("erro no bind\n");
    shutdown(s, STOP_RECEPTION_TRANSMISSION);
    exit(1);
  }

  std::cout << "aqui1" << std::endl;

  // seta informacoes IP/Porta do servidor remoto
  s_serv.sin_family = AF_INET;
  s_serv.sin_addr.s_addr = inet_addr(host.c_str());
  s_serv.sin_port = htons(port);

  // connecta socket aberto no cliente com o servidor
  if(connect(s, (struct sockaddr*)&s_serv, sizeof(s_serv)) != 0)
  {
    //printf("erro na conexao - %d\n", WSAGetLastError());
    printf("erro na conexao");
    shutdown(s, STOP_RECEPTION_TRANSMISSION);
    exit(1);
  }

  std::cout << "aqui2" << std::endl;





}

Socket::~Socket()
{

}