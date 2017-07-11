#include "SharedValues.h"
#include "../Utils.h"
#include "../Message.h"
#include "../Connection.h"

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

std::map<std::string, std::vector<FuncStorage>> serverDb;


extern "C" int rpcRegister(const char* name, int* argTypes, skeleton f) {
  //std::cout << "rpcRegister" << std::endl;
  int result = 0;

  if (server_status < 1) {
    //std::cerr << "Error: the server has not been initialized." << std::endl;
    return -1;
  }

  if (server_status > 2) {
    //std::cerr << "Error: the server is executing." << std::endl;
    return -2;
  }

  server_status = 2;

  
  std::string str_name = name;
  Message msg(REGISTER, serverAddr, clientsPort, str_name, argTypes);
  
  if (socketSendMsg(binderSockfd, MSG_BINDER_SERVER, msg) < 0) {
    return -3;
  }

  // Ack from binder
  Message msg1;
  int buf_size, buf_type; 
  if (socketRecvMsg(binderSockfd, buf_size, buf_type, msg1) < 0) {
    return -4;
  }

  // store in local database
  if (msg1.mType == REGISTER_SUCCESS) {
    pthread_mutex_lock(&serverMutex);
    FuncStorage funcStorage(str_name, msg.argTypes);
    int index = funcStorage.findInDb(serverDb);
    if (index < 0) {
      funcStorage.setSkeleton(f);
      serverDb[str_name].push_back(funcStorage);
    } else {
      serverDb[str_name][index].setSkeleton(f);    // override previous registration
      result = 1;
    }
    pthread_mutex_unlock(&serverMutex);
  } 

  if (msg1.mType == REGISTER_FAILURE) {
    result = -5;
  }
  
  return result;

}

