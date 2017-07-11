#include "../Utils.h"
#include "../Connection.h"
#include <iostream>

extern "C" int rpcCall(const char* name, int* argTypes, void** args) {
  //std::cout << "rpcCall: " << args << std::endl;
 
  int sockfd;

  if ( getenv("BINDER_ADDRESS") == NULL || getenv("BINDER_PORT") == NULL) {
    //std::cerr << "Error: Cannot find the binder info" << std::endl;
    return -1;
  }

  std::string binder_address = getenv("BINDER_ADDRESS");
  std::string binder_port = getenv("BINDER_PORT");

  //int clientsPort;

  if (socketConnect(sockfd, binder_address, binder_port) < 0) {
    return -2;
  }

  // send to binder:
  std::string tmp_name = name;

  Message msg(LOC_REQUEST, tmp_name, argTypes);

  if (socketSendMsg(sockfd, MSG_BINDER_CLIENT, msg) < 0) {
    socketClose(sockfd);
    //std::cerr << "Error: send to binder failed" << std::endl;
    return -3;
  }

  // recv from binder:
  Message msg1;
  int buf_size, buf_type; 
  if (socketRecvMsg(sockfd, buf_size, buf_type, msg1) < 0) {
    socketClose(sockfd);
    //std::cerr << "Error: recv from binder failed" << std::endl;
    return -4;
  }  

  socketClose(sockfd);

  if (msg1.mType == LOC_FAILURE) {
    //std::cerr << "Error: fail to locate the func from binder" << std::endl;
    return -5;
  }

  std::string server_name = msg1.serverId;
  std::string server_port = std::to_string(msg1.serverPort);

  //std::cout << "round robin" << server_name << std::endl;
 
  // Connect to the server
  if (socketConnect(sockfd, server_name, server_port) < 0) {
    return -6;
  }

  Message msg2(EXECUTE, tmp_name, argTypes, args);
  if (socketSendMsg(sockfd, MSG_CLIENT_SERVER, msg2) < 0) {
    socketClose(sockfd);
    //std::cerr << "Error: fail to send to server" << std::endl;
    return -7;
  }

  // Receive call back
  Message msg3;
  msg3.setArgs(args);  
  if (socketRecvMsg(sockfd, buf_size, buf_type, msg3) < 0) {
    socketClose(sockfd);
    //std::cerr << "Error: fail to recv from server" << std::endl;
    return -8;
  }

  socketClose(sockfd);

  if (msg3.mType != EXECUTE_SUCCESS) {    
    return -9;
  }

  return 0;
}

