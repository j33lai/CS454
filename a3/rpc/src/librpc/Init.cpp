#include <iostream>
#include <string>
#include "SharedValues.h"
#include "../Connection.h"

int binderSockfd; 
int clientsSockfd;
int clientsPort;
std::string serverAddr;
pthread_mutex_t serverMutex;
int server_status = 0;

extern "C" int rpcInit() {

  std::cout << "rpcInit" << std::endl;
  if (server_status > 0) {
    std::cout << "A server has already been initialized." << std::endl;
    return -1;
  } else {
    server_status = 1;
  }

  
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
