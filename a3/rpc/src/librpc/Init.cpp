#include <iostream>
#include <string>
#include "SharedValues.h"
#include "../Connection.h"

int binderSockfd; 
int clientsSockfd;
int clientsPort;
std::string serverAddr;
pthread_mutex_t serverMutex;

extern "C" int rpcInit() {

  std::cout << "rpcInit" << std::endl;
  
  pthread_mutex_init(&serverMutex, NULL);  // init mutex


  //std::cout << "initial value " << clientsSockfd << std::endl;

  std::string binder_address = getenv("BINDER_ADDRESS");
  std::string binder_port = getenv("BINDER_PORT");

  
  socketInit(clientsSockfd, serverAddr, clientsPort);
  socketConnect(binderSockfd, binder_address, binder_port);


  if (serverAddr == "" || clientsPort == 0) {
    return -1;
  }


  //socketClose(clientsSockfd);
  //socketClose(binderSockfd);

  return 0;
}
