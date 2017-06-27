#include "Connection.h"

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


void socketInit(int & sockfd, std::string & host, int & port) {
  int status;
  int yes = 1;
  struct addrinfo hints, *servinfo, *p;
  //char ipstr[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  //hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  // 37564
  if ((status = getaddrinfo(NULL, "0", &hints, &servinfo)) != 0) {
    std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
    exit(1);
  }

  for(p = servinfo;p != nullptr; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      std::cerr << "server: socket" << std::endl;
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      std::cerr << "setsockopt" << std::endl;
      exit(1);
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      std::cerr << "server: bind" << std::endl;
      continue;
    } else {
      struct sockaddr_storage addr;
      socklen_t len = sizeof addr;

      getsockname(sockfd, (struct sockaddr*)&addr, &len);
      //getpeername(sockfd, (struct sockaddr*)&addr, &len);

      if (addr.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&addr;
        //inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
        port = ntohs(s->sin_port);
        //std::cout << "ipv4: " << s->sin_port << std::endl;
      } else {
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
        //inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
        port = ntohs(s->sin6_port);
        //std::cout << "ipv6: " << s->sin6_port << std::endl; 
      }

      char hostname[128];
      gethostname(hostname, sizeof hostname);

      host = std::string(hostname);      
      //std::cout << "BINDER_ADDRESS " << hostname << std::endl;
      //std::cout << "BINDER_PORT " << port << std::endl;


    }

    break;
  }

  freeaddrinfo(servinfo);

  if (p == nullptr)  {
    std::cerr << "server: failed to bind" <<std::endl;
    exit(1);
  }

  if (listen(sockfd, BACKLOG) == -1) {
    std::cerr << "listen" << std::endl;
    exit(2);
  }

}

void socketClose(int sockfd) {
  close(sockfd);
}

void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


 
