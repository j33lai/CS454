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
  
  if (socketConnect(sockfd, binder_address, binder_port) < 0) {
    return -2;
  }

  // send to binder:

  Message msg(LOC_REQUEST, func_name, argTypes);
  msg.setReasonCode(1);    // indicate this is a cache call request
 
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

  if (msg1.mType == LOC_FAILURE) {
    socketClose(sockfd);
    //std::cout << "Error: fail to locate the func from binder" << std::endl;
    return -5;
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

  socketClose(sockfd);
  return result; 
}

std::pair<std::string, int> getServer(std::string func_name, int* argTypes, int & reason_code) {
  std::vector<int> arg_types = Message::getArgTypesVector(argTypes);

  int func_id = cachedDb.findFunc(func_name, arg_types);
  if (func_id < 0) {
    // to be commented
    std::cout << "call binder: " << func_name << std::endl;
    reason_code = requestToBinder(func_name, argTypes);
    if (reason_code < 0) {
      return std::pair<std::string, int>("", 0);  // request to binder failed 
    }
    func_id = cachedDb.findFunc(func_name, arg_types);  
  } 
  return cachedDb.getServerRR(func_name, func_id); 
}

int requestToServer(std::string func_name, std::string server_name, int server_port, int* argTypes, void** args) {

  int sockfd;

  // Connect to the server
  if (socketConnect(sockfd, server_name, std::to_string(server_port)) < 0) {
    return -1;
  }

  Message msg2(EXECUTE, func_name, argTypes, args);
  if (socketSendMsg(sockfd, MSG_CLIENT_SERVER, msg2) < 0) {
    socketClose(sockfd);
    std::cerr << "send to server failed" << std::endl;
    return -2;
  }

  // Receive call back
  Message msg3;
  msg3.setArgs(args);
  int buf_size, buf_type;
  if (socketRecvMsg(sockfd, buf_size, buf_type, msg3) < 0) {
    socketClose(sockfd);
    std::cerr << "recv to binder failed" << std::endl;
    return -3;
  }

  socketClose(sockfd);

  if (msg3.mType != EXECUTE_SUCCESS) {
    return 1;
  }

  return 0;
}

extern "C" int rpcCacheCall(const char* name, int* argTypes, void** args) {
  //std::cout << "rpcCacheCall" << std::endl;
  int result = 0; 
 
  if ( getenv("BINDER_ADDRESS") == NULL || getenv("BINDER_PORT") == NULL) {
    //std::cerr << "Error: Cannot find the binder info" << std::endl;
    return -1;
  }

  std::string func_name = name;

  std::pair<std::string, int> server_info = getServer(func_name, argTypes, result);

  //std::cout << server_info.first << std::endl;

  if (result < 0) {
    return result;
  }

  while (true) {
    int result_from_server = requestToServer(func_name, server_info.first, server_info.second, argTypes, args);
    if (result_from_server < 0) {
      cachedDb.removeServer(server_info.first, server_info.second);
      server_info = getServer(func_name, argTypes, result);
      if (result < 0) {
        return result;
      }
    } else {
      if (result_from_server == 1) {
        result = -6;
      }
      break;
    }
  }

  return result;
}
