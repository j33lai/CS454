#include <iostream>
#include <string>
#include "SharedValues.h"
#include "../Connection.h"

int binderSockfd; 
int clientsSockfd;
int clientsPort;
std::string serverAddr;

extern "C" int rpcInit() {

  std::cout << "rpcInit" << std::endl;
  //std::cout << "initial value " << clientsSockfd << std::endl;

  //int clientsSockfd, binderSockfd;
  std::string binder_address = getenv("BINDER_ADDRESS");
  std::string binder_port = getenv("BINDER_PORT");

  //int clientsPort;
  
  socketInit(clientsSockfd, serverAddr, clientsPort);
  socketConnect(binderSockfd, binder_address, binder_port);

  //std::cout << "ADDRESS " << host << std::endl;
  //std::cout << "CLIENTS_PORT " << clientsPort << std::endl;
  

  if (serverAddr == "" || clientsPort == 0) {
    return -1;
  }


  //socketClose(clientsSockfd);
  //socketClose(binderSockfd);

  return 0;
}
