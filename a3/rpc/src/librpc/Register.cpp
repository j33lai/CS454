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
  std::cout << "rpcRegister" << std::endl;
  
  std::string str_name = name;
  Message msg(REGISTER, serverAddr, clientsPort, str_name, argTypes);
  int result = socketSendMsg(binderSockfd, MSG_BINDER_SERVER, msg);

  if (result >=0) {
    std::cout << "register: " << *f << std::endl;
    pthread_mutex_lock(&serverMutex);
    FuncStorage funcStorage(str_name, msg.argTypes);
    int index = funcStorage.findInDb(serverDb);
    if (index < 0) {
      funcStorage.setSkeleton(f);
      serverDb[str_name].push_back(funcStorage);
    } else {
      serverDb[str_name][index].setSkeleton(f);    // override previous registration
    }
    pthread_mutex_unlock(&serverMutex);
  }

  return result;

}

