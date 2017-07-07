#include "../Utils.h"
#include "../Connection.h"
#include <iostream>

extern "C" int rpcTerminate() {
  std::cout << "rpcTerminate" << std::endl;
  
  int sockfd;
  std::string binder_address = getenv("BINDER_ADDRESS");
  std::string binder_port = getenv("BINDER_PORT");
 
  socketConnect(sockfd, binder_address, binder_port);
   
  Message msg(TERMINATE);
  
  if (socketSendMsg(sockfd, MSG_BINDER_CLIENT, msg) < 0) {
    socketClose(sockfd);
    std::cout << "send to binder failed" << std::endl;
    return -1;
  }
  
  return 0;
}

