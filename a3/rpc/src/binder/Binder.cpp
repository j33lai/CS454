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
  //std::cout << "Binder:" << std::endl;
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
    if (terminating && funcDatabase.getServerList().size() == 0) {
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
  //std::cout << "handle client" << std::endl;

  if (!connected) {  // client has closed connection
    return;
  }
  int reason_code = 0;

  // Termination
  if (fdToMsg[new_fd].mType == TERMINATE) {
    handleTermination();
    return;
  }

  int func_id = -1;
  std::string func_name;

  if (terminating) {
    reason_code = -2;    // the binder is terminating
  } else {
    func_name = fdToMsg[new_fd].funcName;
    func_id = funcDatabase.findFunc(fdToMsg[new_fd].funcName, fdToMsg[new_fd].argTypes);
    reason_code = func_id < 0? func_id : reason_code;
  }

  int result;

  if (func_id >= 0) {
    if (fdToMsg[new_fd].reasonCode == 1) {
      // Handle cache call request from client
      std::vector<std::pair<std::string, int>> func_servers = funcDatabase.getFunc(func_name, func_id).getServers();
      int num_servers = func_servers.size();
      Message msg(LOC_SUCCESS, func_servers[0].first, func_servers[0].second);
      // Include the number of servers
      msg.setReasonCode(num_servers);      
      result = socketSendMsg(new_fd, MSG_CLIENT_SERVER, msg);
      for (int i = 1; i < num_servers; i++) {
        Message msg1(LOC_SUCCESS, func_servers[i].first, func_servers[i].second);
        result = socketSendMsg(new_fd, MSG_CLIENT_SERVER, msg1); 
      } 
    } else {
      // Handle usual call request from client
      // provide server info to client based on round robin
      std::pair<std::string, int> server_info = funcDatabase.getServerRR(func_name, func_id);
      std::string server_name = server_info.first;
      int server_port = server_info.second;    

      // without round robin
      //std::string server_name = binderFuncs[fdToMsg[new_fd].funcName][func_id].getServerName();
      //int server_port = binderFuncs[fdToMsg[new_fd].funcName][func_id].getServerPort();

      Message msg(LOC_SUCCESS, server_name, server_port);
      result = socketSendMsg(new_fd, MSG_CLIENT_SERVER, msg);
    }
  } else {
    Message msg(LOC_FAILURE, reason_code);
    result = socketSendMsg(new_fd, MSG_CLIENT_SERVER, msg);
  }

  if (result < 0) {
    std::cout << "Binder handle client failed" << std::endl;
  }
}


void Binder::handleServer(int new_fd, bool connected) {
  //std::cout << "handle server" << std::endl;

  Message msg = fdToMsg[new_fd];

  if (!connected) {  // server has closed connection
    funcDatabase.removeServer(msg.serverId, msg.serverPort);
    return;
  }
  
  int reason_code = 0; 

  if (terminating) {
    reason_code = -2;  
  } else {
    reason_code = funcDatabase.addFunc(msg.funcName, msg.argTypes, msg.serverId, msg.serverPort);   
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
  std::vector<std::pair<std::string, int>> serverList = funcDatabase.getServerList();
  for (unsigned i = 0; i < serverList.size(); i++) {
    int new_fd;
    socketConnect(new_fd, serverList[i].first, std::to_string(serverList[i].second));
    Message msg(TERMINATE);
    socketSendMsg(new_fd, MSG_BINDER_SERVER, msg);
  }
}


