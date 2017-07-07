#include "Binder.h"
#include "../Utils.h"
#include "../Connection.h"

#include <iostream>
#include <sstream>
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


Binder::Binder() {
  std::cout << "Binder:" << std::endl;
}

void Binder::binderInit() {
  std::string host;
  int port;
  socketInit(sockfd, host, port);
  std::cout << "BINDER_ADDRESS " << host << std::endl;
  std::cout << "BINDER_PORT " << port << std::endl; 
}


void Binder::binderAccept() {
  int new_fd;
  struct sockaddr_storage their_addr;
  socklen_t sin_size;
  char ipstr[INET6_ADDRSTRLEN];

  fd_set master;
  fd_set read_fds;
  int fdmax = sockfd;
  FD_ZERO(&master);
  FD_ZERO(&read_fds);
  FD_SET(sockfd, &master);
  
  while (true) {
    if (terminating && serverList.size() == 0) {
      break;
    }

    read_fds = master;
    if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
      std::cerr << "ERROR: select" << std::endl;
      exit(3);
    }

    for (int i = 0; i < fdmax + 1; i++) {
      if (FD_ISSET(i, &read_fds)) {
        if (i == sockfd) {
          sin_size = sizeof their_addr;
          new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
          if (new_fd == -1) {
            std::cerr << "ERROR: accept" << std::endl;
          } else {
            FD_SET(new_fd, &master);
            fdToSize[new_fd] = 0;
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
            handle(i, false);
            fdToSize.erase(i);
            fdToType.erase(i);
            fdToBuf.erase(i);
            fdToMsg.erase(i);
            close(i);
            FD_CLR(i, &master);
          } else {
            handle(i, true);
          }
        }
      }
    }

  }

}

void Binder::binderTerminate() {
  close(sockfd);
}

int Binder::dealWith(int new_fd) {
  Message msg;
  int buf_size, buf_type, result; 
  result = socketRecvMsg(new_fd, buf_size, buf_type, msg);
  if (result >= 0) {
    fdToSize[new_fd] = buf_size;
    fdToType[new_fd] = buf_type;
    fdToMsg[new_fd] = msg;
    //fdToBuf[new_fd] = strcpy(fdToBuf[new_fd], msg.funcName.c_str());
  } 

  return result;
 
}

void Binder::handle(int new_fd, bool connected) {
  switch(fdToType[new_fd]) {
    case MSG_BINDER_CLIENT:
      handleClient(new_fd, connected);
      break;
    case MSG_BINDER_SERVER:
      handleServer(new_fd, connected);
      break;
    default:
      break;
  }
}

void Binder::handleClient(int new_fd, bool connected) {
  std::cout << "handle client" << std::endl;

  if (!connected) {  // client has closed connection
    return;
  }
  int result;

  // Termination
  if (fdToMsg[new_fd].mType == TERMINATE) {
    handleTermination();
    return;
  }
 
  std::string func_name = fdToMsg[new_fd].funcName;
  int func_id = hasFunc(fdToMsg[new_fd].funcName, fdToMsg[new_fd].argTypes);

 
  if (func_id >= 0) {
    // provide server info to client based on round robin
    std::pair<std::string, int> server_info = getServerRR(func_name, func_id);
    std::string server_name = server_info.first;
    int server_port = server_info.second;    

    // without round robin
    //std::string server_name = binderFuncs[fdToMsg[new_fd].funcName][func_id].getServerName();
    //int server_port = binderFuncs[fdToMsg[new_fd].funcName][func_id].getServerPort();

    Message msg(LOC_SUCCESS, server_name, server_port);
    result = socketSendMsg(new_fd, MSG_CLIENT_SERVER, msg);
  } else {
    Message msg(LOC_FAILURE, -1);
    result = socketSendMsg(new_fd, MSG_CLIENT_SERVER, msg);
  }
  if (result < 0) {
    std::cout << "Binder handle client failed" << std::endl;
  }
}


void Binder::handleServer(int new_fd, bool connected) {
  std::cout << "handle server" << std::endl;
  if (!connected) {  // server has closed connection
    removeServer(new_fd); 
    return;
  }

  Message msg = fdToMsg[new_fd];

  // testing code to be removed
  std::cout 
    << msg.serverId << " " 
    << msg.serverPort << " " 
    << msg.funcName << " "
    << std::endl;
  // testing code to be removed

  int reason_code = 0; 

  if (terminating) {
    reason_code = -2;  
  } else {
    reason_code = addFunc(msg);   
  }
 
  if (reason_code == 0) {
    addServer(msg.serverId, msg.serverPort);    // add server info to the list
  }

  if (reason_code >=0) {
    // Ack success
    Message msg1(REGISTER_SUCCESS, reason_code);
    socketSendMsg(new_fd, MSG_BINDER_CLIENT, msg1); 
  } else {
    // Ack failure
    Message msg2(REGISTER_FAILURE, reason_code);
    socketSendMsg(new_fd, MSG_BINDER_CLIENT, msg2);
  }
}

void Binder::handleTermination() {
  terminating = true;
  for (unsigned i = 0; i < serverList.size(); i++) {
    int new_fd;
    socketConnect(new_fd, serverList[i].first, std::to_string(serverList[i].second));
    Message msg(TERMINATE);
    socketSendMsg(new_fd, MSG_BINDER_SERVER, msg);
  }
}

int Binder::hasFunc(std::string name, std::vector<int> argTypes) {
  if (binderFuncs.find(name) != binderFuncs.end()) {
     int binder_func_name_size = binderFuncs[name].size();
     for (int i = 0; i < binder_func_name_size; i++) {
       std::cout << binderFuncs[name][i].fName << std::endl;
       if (binderFuncs[name][i].equalToFunc(name, argTypes)) {
          return i;
       }
     }
  }
  return -1;
}

int Binder::addFunc(const Message & msg) {
  int reason_code = 0;
  try {
    int func_id = hasFunc(msg.funcName, msg.argTypes);
    if (func_id < 0) {
      FuncStorage funcStorage(msg.funcName, msg.argTypes);
      funcStorage.addServer(msg.serverId, msg.serverPort);
      binderFuncs[msg.funcName].push_back(funcStorage);
    } else {
      if (binderFuncs[msg.funcName][func_id].addServer(msg.serverId, msg.serverPort) > 0) {
        reason_code = 1;
      }
    }
  } catch (...) {
    reason_code = -1;
  }
  return reason_code;
}

void Binder::addServer(std::string name, int port) {
  for (unsigned i = 0; i < serverList.size(); i++) {
    if (serverList[i].first == name && serverList[i].second == port) {
      return;  // server is already registered
    }
  }
  // add server
  std::cout << "add server" << std::endl;
  std::pair<std::string, int> new_server(name, port);
  serverList.push_back(new_server);
}

void Binder::removeServer(std::string name, int port) {
  int rr_size = serverList.size();
  for (int i = 0; i < rr_size; i++) {
    if (serverList[i].first == name && serverList[i].second == port) {          
      if (i <= rr_id) {
        rr_id = (rr_id + rr_size - 1) % rr_size;
      } 
      serverList.erase(serverList.begin() + i);
      break;  
    }
  }
}

void Binder::removeServer(int fd) {
  std::string server_name = fdToMsg[fd].serverId;
  int server_port = fdToMsg[fd].serverPort;
  for(auto & vs : binderFuncs) {
    for(auto & v : vs.second) {
      v.removeServer(server_name, server_port);
    }
  }
  removeServer(server_name, server_port);
}



std::pair<std::string, int> Binder::getServerRR(std::string func_name, int func_id) { 
  FuncStorage funcStorage = binderFuncs[func_name][func_id];
  int rr_size = serverList.size();
  int i = (rr_id + 1) % rr_size;
  while (true) {
    int id = funcStorage.hasServer(serverList[i].first, serverList[i].second);
    if (id >= 0) {
      rr_id = i;
      return funcStorage.getServer(id); 
    }
    i = (i + 1) % rr_size;
  }  
}

