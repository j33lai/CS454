#include "../Utils.h"
#include "../Connection.h"
#include <iostream>

extern "C" int rpcTerminate() {
  //std::cout << "rpcTerminate" << std::endl;

  if ( getenv("BINDER_ADDRESS") == NULL || getenv("BINDER_PORT") == NULL) {
    //std::cerr << "Error: Cannot find the binder info" << std::endl;
    return -1;
  }
  
  int sockfd;
  std::string binder_address = getenv("BINDER_ADDRESS");
  std::string binder_port = getenv("BINDER_PORT");
 
  if (socketConnect(sockfd, binder_address, binder_port) < 0) {
    return -2;
  }
   
  Message msg(TERMINATE);
  
  if (socketSendMsg(sockfd, MSG_BINDER_CLIENT, msg) < 0) {
    socketClose(sockfd);
    //std::cerr << "Error: send to binder failed" << std::endl;
    return -3;
  }
  
  return 0;
}

