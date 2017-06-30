#include "../Utils.h"
#include "../Connection.h"
#include <iostream>

extern "C" int rpcCall(const char* name, int* argTypes, void** args) {
  std::cout << "rpcCall" << std::endl;
 
  int sockfd;
  std::string binder_address = getenv("BINDER_ADDRESS");
  std::string binder_port = getenv("BINDER_PORT");

  //int clientsPort;

  socketConnect(sockfd, binder_address, binder_port);

  // send to binder:
  std::string tmp_name = name;

  int buf_size = tmp_name.length() + 1;
  int buf_type = MSG_BINDER_CLIENT;
  if (socketSend(sockfd, buf_type, tmp_name) < 0) {
    socketClose(sockfd);
    return -1;
  }

  // recv from binder:
  std::string recv_msg;  

  if (socketRecv(sockfd, buf_size, buf_type, recv_msg) < 0) {
    socketClose(sockfd);
    return -1;
  }

  socketClose(sockfd);

  std::vector<std::string> server_info = split(recv_msg, ' ');
  if (server_info.size() < 2) {
    std::cout << "not registered in the binder" << std::endl;
    return -1;
  }

  std::cout << "server info for " << name << " " << server_info[0] << " " << server_info[1] << std::endl;

  // connect to server
  socketConnect(sockfd, server_info[0], server_info[1]);

  buf_size = tmp_name.length() + 1;
  buf_type = MSG_CLIENT_SERVER;

  if (socketSend(sockfd, buf_type, tmp_name) < 0) {
    std::cout << "fail to send to server" << std::endl;
    socketClose(sockfd);
    return -1;
  } else {
    std::cout << "connected to server" << std::endl;
  }

  socketClose(sockfd);

  return 1;
}

