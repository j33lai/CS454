#include <iostream>
#include "../Connection.h"

extern "C" int rpcInit() {

  std::cout << "rpcInit" << std::endl;


  int sockfd;
  std::string host = "";
  int port = 0;
  socketInit(sockfd, host, port);
  std::cout << "SERVER_ADDRESS " << host << std::endl;
  std::cout << "SERVER_PORT " << port << std::endl;

  if (host == "" || port == 0) {
    return -1;
  }

  socketClose(sockfd);
  return 0;
}
