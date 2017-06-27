#include "Binder.h"
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
            fdToSize.erase(i);
            close(i);
            FD_CLR(i, &master);
          } else {
            fdToSize[i] = size;
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
  int numbytes;

  int buf_size, buf_type;
  if ((numbytes = recv(new_fd, &buf_size, 4, 0)) <= 0) {
    //std::cerr << "Connection " << new_fd << " is closed." << std::endl;
    return -1;
  } 

  if ((numbytes = recv(new_fd, &buf_type, 4, 0)) <= 0) {
    //std::cerr << "Connection " << new_fd << " is closed." << std::endl;
    return -1;
  } else {
    fdToType[new_fd] = buf_type;
    fdToBuf[new_fd] = new char[buf_size];
    return buf_size;
  }
}

void Binder::handleClient() {
  std::cout << "handle client" << std::endl;
}


void Binder::handleServer() {
  std::cout << "handle server" << std::endl;
}







