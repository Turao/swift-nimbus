#include "NimbusClient.h"
#include "Server.h"

int main(int argc, char* argv[])
{
  if(argc < 2) {
    Server *server = new Server();
  }
  else {
    int port = atoi(argv[1]);
    Server *server = new Server(port); 


  while(1)
  {
    std::this_thread::sleep_for (std::chrono::seconds(1));
  }
  }
}