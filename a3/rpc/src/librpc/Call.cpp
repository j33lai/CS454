#include "../Utils.h"
#include "../Connection.h"
#include <iostream>

extern "C" int rpcCall(const char* name, int* argTypes, void** args) {
  //std::cout << "rpcCall: " << args << std::endl;
 
  int sockfd;
  std::string binder_address = getenv("BINDER_ADDRESS");
  std::string binder_port = getenv("BINDER_PORT");

  //int clientsPort;

  socketConnect(sockfd, binder_address, binder_port);

  // send to binder:
  std::string tmp_name = name;

  Message msg(LOC_REQUEST, tmp_name, argTypes);

  if (socketSendMsg(sockfd, MSG_BINDER_CLIENT, msg) < 0) {
    socketClose(sockfd);
    std::cout << "send to binder failed" << std::endl;
    return -1;
  }

  // recv from binder:
  Message msg1;
  int buf_size, buf_type; 
  if (socketRecvMsg(sockfd, buf_size, buf_type, msg1) < 0) {
    socketClose(sockfd);
    std::cout << "recv from binder failed" << std::endl;
    return -1;
  }  

  socketClose(sockfd);

  if (msg1.mType == LOC_FAILURE) {
    std::cout << "Did not find the func from binder" << std::endl;
    return -1;
  }

  std::string server_name = msg1.serverId;
  std::string server_port = std::to_string(msg1.serverPort);

  //std::cout << "round robin" << server_name << std::endl;
 
  // Connect to the server
  socketConnect(sockfd, server_name, server_port);

  Message msg2(EXECUTE, tmp_name, argTypes, args);
  if (socketSendMsg(sockfd, MSG_CLIENT_SERVER, msg2) < 0) {
    socketClose(sockfd);
    std::cout << "send to server failed" << std::endl;
    return -1;
  }

  // Receive call back
  Message msg3;
  msg3.setArgs(args);  
  if (socketRecvMsg(sockfd, buf_size, buf_type, msg3) < 0) {
    socketClose(sockfd);
    std::cout << "recv to binder failed" << std::endl;
    return -1;
  }

/*
  if (msg3.mType == EXECUTE_SUCCESS) {
    
    std::cout << "SUCCESS: " << args << std::endl;

  }
*/

  socketClose(sockfd);

  return 1;
}

