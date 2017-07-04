#include "SharedValues.h"
#include "../Utils.h"
#include "../Connection.h"

#include <iostream>
#include <map>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

std::map<int, int> server_fdToSize;
std::map<int, int> server_fdToType;
std::map<int, Message> server_fdToMsg;

int dealWith(int new_fd) {
  Message msg;
  int buf_size, buf_type, result;
  result = socketRecvMsg(new_fd, buf_size, buf_type, msg);
  if (result >= 0) {
    server_fdToSize[new_fd] = buf_size;
    server_fdToType[new_fd] = buf_type;
    server_fdToMsg[new_fd] = msg;
  }

  return result;  
}

void handleClient(int new_fd) {
  // Execute func and send back to client
  int result = -1;
  std::string func_name = server_fdToMsg[new_fd].funcName;

  FuncStorage funcStorage(func_name, server_fdToMsg[new_fd].argTypes);
  int func_id = funcStorage.findInDb(serverDb);

  if (func_id >= 0) {
    skeleton fSkeleton = serverDb[func_name][func_id].fSkeleton;
    int *arg_types = server_fdToMsg[new_fd].getArgTypesPointer();
    void **args = server_fdToMsg[new_fd].mArgs;
   
    // invoke method on the server side 
    result = fSkeleton(arg_types, args);
    if (result >= 0) {
      Message msg(EXECUTE_SUCCESS, func_name, arg_types, args);
      if (socketSendMsg(new_fd, MSG_CLIENT_SERVER, msg) < 0) {
        result = -1;
      }
    } 
  } 

  if (result < 0) { 
    Message msg(EXECUTE_FAILURE, -1);
    if (socketSendMsg(new_fd, MSG_CLIENT_SERVER, msg) < 0) {
      result = -1;
    }
  }
  server_fdToMsg[new_fd].deleteArgs();
}


void handleBinder(int new_fd) {
  std::cout << "handle binder" << std::endl;
}


void handle(int new_fd) {
  switch(server_fdToType[new_fd]) {
    case MSG_CLIENT_SERVER:
      handleClient(new_fd);
      break;
    case MSG_BINDER_SERVER:
      handleBinder(new_fd);
      break;
    default:
      break;
  }
}

extern "C" int rpcExecute() {
  std::cout << "rpcExecute" << std::endl;

  int new_fd;
  struct sockaddr_storage their_addr;
  socklen_t sin_size;
  char ipstr[INET6_ADDRSTRLEN];

  fd_set master;
  fd_set read_fds;
  int fdmax = clientsSockfd;
  FD_ZERO(&master);
  FD_ZERO(&read_fds);
  FD_SET(clientsSockfd, &master);

  while (true) {
    read_fds = master;
    if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
      std::cerr << "ERROR: select" << std::endl;
      exit(3);
    }

    for (int i = 0; i < fdmax + 1; i++) {
      if (FD_ISSET(i, &read_fds)) {
        if (i == clientsSockfd) {
          sin_size = sizeof their_addr;
          new_fd = accept(clientsSockfd, (struct sockaddr *)&their_addr, &sin_size);
          if (new_fd == -1) {
            std::cerr << "ERROR: accept" << std::endl;
          } else {
            FD_SET(new_fd, &master);
            server_fdToSize[new_fd] = 0;
            if (new_fd > fdmax) {
              fdmax = new_fd;
            }

            inet_ntop(their_addr.ss_family,
              get_in_addr((struct sockaddr *)&their_addr),
              ipstr, sizeof ipstr);
          }
        } else {
          int size = dealWith(i);
          if (size < 0) {
            server_fdToSize.erase(i);
            server_fdToType.erase(i);
            server_fdToMsg.erase(i); 
            close(i);
            FD_CLR(i, &master);
          } else {
            handle(i);
          }
        }
      }
    }

  }
    
  return 1;
}

