#include <iostream>
#include "SharedValues.h"
#include "../Connection.h"


extern "C" int rpcInit() {

  std::cout << "rpcInit" << std::endl;
  std::cout << "initial value " << clientsSockfd << std::endl;

  //int clientsSockfd, binderSockfd;
  std::string host = "";
  std::string binder_address = getenv("BINDER_ADDRESS");
  std::string binder_port = getenv("BINDER_PORT");

  int clientsPort;
  
  socketInit(clientsSockfd, host, clientsPort);
  socketConnect(binderSockfd, binder_address, binder_port);

  std::cout << "ADDRESS " << host << std::endl;
  std::cout << "CLIENTS_PORT " << clientsPort << std::endl;
  

  if (host == "" || clientsPort == 0) {
    return -1;
  }

  std::cout << "after value " << clientsSockfd << std::endl;

  while (true) {}

  //socketClose(clientsSockfd);
  //socketClose(binderSockfd);

  return 0;
}
