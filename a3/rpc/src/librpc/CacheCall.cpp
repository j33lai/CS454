#include "../Utils.h"
#include "../Message.h"
#include "../Connection.h"
#include "../FuncStorage.h"
#include "../FuncDatabase.h"
#include <iostream>
#include <map>
#include <utility>

FuncDatabase cachedDb;

int requestToBinder(std::string func_name, int* argTypes) {
  int result = 0;

  int sockfd;
  std::string binder_address = getenv("BINDER_ADDRESS");
  std::string binder_port = getenv("BINDER_PORT");
  
  socketConnect(sockfd, binder_address, binder_port);

  // send to binder:

  Message msg(LOC_REQUEST, func_name, argTypes);
  msg.setReasonCode(1);    // indicate this is a cache call request
 
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

  if (msg1.mType == LOC_FAILURE) {
    socketClose(sockfd);
    std::cout << "Did not find the func from binder" << std::endl;
    return -1;
  }

  int num_servers = msg1.reasonCode;
  int num_servers_recv = 0;
  std::vector<std::pair<std::string, int>> server_list;

  // push the first server into the list
  server_list.push_back(std::pair<std::string, int>(msg1.serverId, msg1.serverPort));

  for (int i = 1; i < num_servers; i++) {
    Message msg_i;
    if (socketRecvMsg(sockfd, buf_size, buf_type, msg_i) >= 0) {
      // add server to the list;
      server_list.push_back(std::pair<std::string, int>(msg_i.serverId, msg_i.serverPort));
      num_servers_recv++;
    }
  }


  if (num_servers_recv < num_servers - 1) {
    result = 1;     // binder failed to send all server info
  }  

  FuncStorage funcStorage(msg.funcName, msg.argTypes);
  cachedDb.addFunc(msg.funcName, msg.argTypes, server_list);


  //std::string server_name = msg1.serverId;
  //std::string server_port = std::to_string(msg1.serverPort);

  socketClose(sockfd);
  return result; 
}

std::pair<std::string, int> getServer(std::string func_name, int* argTypes) {
  std::vector<int> arg_types = Message::getArgTypesVector(argTypes);

  int func_id = cachedDb.findFunc(func_name, arg_types);
  if (func_id < 0) {
    std::cout << "call binder: " << func_name << std::endl;
    if (requestToBinder(func_name, argTypes) < 0) {
      return std::pair<std::string, int>("", 0);  // request to binder failed 
    }
    func_id = cachedDb.findFunc(func_name, arg_types);  
  } 
  return cachedDb.getServerRR(func_name, func_id); 
}

int requestToServer(std::string func_name, std::string server_name, int server_port, int* argTypes, void** args) {

  int sockfd;

  // Connect to the server
  socketConnect(sockfd, server_name, std::to_string(server_port));

  Message msg2(EXECUTE, func_name, argTypes, args);
  if (socketSendMsg(sockfd, MSG_CLIENT_SERVER, msg2) < 0) {
    socketClose(sockfd);
    std::cout << "send to server failed" << std::endl;
    return -1;
  }

  // Receive call back
  Message msg3;
  msg3.setArgs(args);
  int buf_size, buf_type;
  if (socketRecvMsg(sockfd, buf_size, buf_type, msg3) < 0) {
    socketClose(sockfd);
    std::cout << "recv to binder failed" << std::endl;
    return -1;
  }

  socketClose(sockfd);

  return 0;
}

extern "C" int rpcCacheCall(const char* name, int* argTypes, void** args) {
  std::cout << "rpcCacheCall" << std::endl;
  
  std::string func_name = name;

  std::pair<std::string, int> server_info = getServer(func_name, argTypes);
  std::cout << server_info.first << std::endl;

  if (server_info.first == "") {
    return -1;
  }

  while (true) {
    if (requestToServer(func_name, server_info.first, server_info.second, argTypes, args) < 0) {
      cachedDb.removeServer(server_info.first, server_info.second);
      server_info = getServer(func_name, argTypes);
      if (server_info.first == "") {
        return -1;
      }
    } else {
      break;
    }
  }

  return 0;
}
