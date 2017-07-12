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
  //std::cout << "rpcInit" << std::endl;

  if (server_status > 0) {
    //std::cout << "A server has already been initialized." << std::endl;
    return 1;
  } else {
    server_status = 1;
  }

  if ( getenv("BINDER_ADDRESS") == NULL || getenv("BINDER_PORT") == NULL) {
    //std::cerr << "Error: Cannot find the binder info" << std::endl;
    return -1;
  }

  
  pthread_mutex_init(&serverMutex, NULL);  // init mutex
  
  if (socketInit(clientsSockfd, serverAddr, clientsPort) < 0) {
    return -2;
  }

  if (serverAddr == "" || clientsPort == 0) {
    return -3;
  }

  // get binder info
  std::string binder_address = getenv("BINDER_ADDRESS");
  std::string binder_port = getenv("BINDER_PORT");

  if (socketConnect(binderSockfd, binder_address, binder_port) < 0) {
    return -4;
  }
 
  return 0;
}
