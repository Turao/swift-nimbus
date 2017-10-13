#include "NimbusClient.h"

#include "Client.h"

#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
  if(argc < 4) {
    cout << "Usage: NimbusClient username address port" << endl;
  }
  else {
    string username = string(argv[1]);
    string address = string(argv[2]);
    int port = atoi(argv[3]);

    Client *client = new Client(username);
  }
}