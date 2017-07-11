#include "Connection.h"
#include "Utils.h"

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


int socketInit(int & sockfd, std::string & host, int & port) {
  int status;
  int yes = 1;
  struct addrinfo hints, *servinfo, *p;
  //char ipstr[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  //hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((status = getaddrinfo(NULL, "0", &hints, &servinfo)) != 0) {
    //std::cerr << "Error: getaddrinfo" << gai_strerror(status) << std::endl;
    return -1;
  }

  for(p = servinfo;p != nullptr; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      //std::cerr << "Error: socket" << std::endl;
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      //std::cerr << "Error: setsockopt" << std::endl;
      return -2;
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      //std::cerr << "Error: bind" << std::endl;
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
      } else {
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
        //inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
        port = ntohs(s->sin6_port);
      }

      char hostname[128];
      gethostname(hostname, sizeof hostname);

      host = std::string(hostname);      
    }

    break;
  }

  freeaddrinfo(servinfo);

  if (p == nullptr)  {
    //std::cerr << "Error: null server info ptr" <<std::endl;
    return -3;
  }

  if (listen(sockfd, BACKLOG) == -1) {
    //std::cerr << "Error: listen" << std::endl;
    return -4;
  }

  return 0;
}

int socketConnect(int & sockfd, std::string destHost, std::string destPort) {
  struct addrinfo hints, *servinfo, *p;
  int status;
  char ipstr[INET6_ADDRSTRLEN];

  const char * dest_address = destHost.c_str();;
  const char * dest_port = destPort.c_str();


  if (dest_address == nullptr || dest_port == nullptr) {
    //std::cerr << "Error: destination address or port not found!" << std::endl;
    return -1;
  }

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((status = getaddrinfo(dest_address, dest_port, &hints, &servinfo)) != 0) {
    //std::cerr << "Error: getaddrinfo" << std::endl;
    return -2;
  }

  for(p = servinfo; p != nullptr; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      //std::cerr << "Failed to socket:0" << std::endl;  
      continue;
    }

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      //std::cerr << "Failed to connect:1" << std::endl;  
      continue;
    }

    break;
  }

  if (p == nullptr) {
    //std::cerr << "Error: null server info ptr" << std::endl;
    return -3;
  }

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), ipstr, sizeof ipstr);

  freeaddrinfo(servinfo);
  return 0;
}


int socketSendMsg(int sockfd, int type, const Message & msg) {
  int buf_size = msg.getSerializationSize();
  int buf_type = type;
  int *buf_ints = new int[2];

  buf_ints[0] = buf_size;
  buf_ints[1] = buf_type;

  char *msg_ser = new char[buf_size];

  Message *tmp_msg = new Message(msg);  

  serializeMessage(tmp_msg, msg_ser);

  int result = 0;
  int numbytes;

  if ((numbytes = send(sockfd, buf_ints, 8, 0)) == -1) {
    //std::cerr << "Sending msg failed." << std::endl;
    result = -1;
  }

  if (result >=0 && (numbytes = send(sockfd, msg_ser, buf_size, 0)) == -1) {
    //std::cerr << "Sending msg failed." << std::endl;
    result = -1;
  }
  delete [] buf_ints;
  delete [] msg_ser;
  delete tmp_msg;
  return result;
}


int socketRecvMsg(int sockfd, int & size, int & type, Message & msg) {
  int numbytes;

  int *buf_ints = new int[2];
  if ((numbytes = recv(sockfd, buf_ints, 8, 0)) <= 0) {
    //std::cerr << "Connection " << sockfd << " is closed." << std::endl;
    delete [] buf_ints;
    return -1;
  }


  size = buf_ints[0];
  type = buf_ints[1];

  char *msg_ser = new char[size];

  numbytes = recv(sockfd, msg_ser, size, 0);

  deserializeMessage(msg_ser, &msg);

  delete [] msg_ser;

  if (numbytes <= 0) {
    //std::cerr << "Connection " << sockfd <<" is closed." << std::endl;
    return -1;
  }

  return size;
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


 
