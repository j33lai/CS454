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

void socketConnect(int & sockfd, std::string destHost, std::string destPort) {
  struct addrinfo hints, *servinfo, *p;
  int status;
  char ipstr[INET6_ADDRSTRLEN];

  const char * dest_address = destHost.c_str();;
  const char * dest_port = destPort.c_str();


  if (dest_address == nullptr || dest_port == nullptr) {
    std::cout << "Destination address or port not found!" << std::endl;
    exit(1);
  }

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((status = getaddrinfo(dest_address, dest_port, &hints, &servinfo)) != 0) {
    std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
    exit(1);
  }

  for(p = servinfo; p != nullptr; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      std::cerr << "Failed to socket:0" << std::endl;  
      continue;
    }

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      std::cerr << "Failed to connect:1" << std::endl;  
      continue;
    }

    break;
  }

  if (p == nullptr) {
    std::cerr << "Failed to connect:2" << std::endl;
    exit(2);
  }

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), ipstr, sizeof ipstr);

  freeaddrinfo(servinfo);

}

int socketSend(int sockfd, int type, std::string msg) {
  int buf_size = msg.length() + 1;
  int buf_type = type;
  char *buf = new char[msg.length() + 1];
  buf = strcpy(buf, msg.c_str());

  int result = 0;
  int numbytes;

  if ((numbytes = send(sockfd, &buf_size, 4, 0)) == -1) {
    std::cerr << "Sending msg failed." << std::endl;
    result = -1;
  }

  if (result >=0 && (numbytes = send(sockfd, &buf_type, 4, 0)) == -1) {
    result = -1;
    std::cerr << "Sending msg failed." << std::endl;
  }

  if (result >=0 && (numbytes = send(sockfd, buf, buf_size, 0)) == -1) {
    result = -1;
    std::cerr << "Sending msg failed." << std::endl;
  }
  delete [] buf;
  return result;
}


int socketRecv(int sockfd, int & size, int & type, std::string & msg) {
  int numbytes;

  int buf_size, buf_type;
  if ((numbytes = recv(sockfd, &buf_size, 4, 0)) <= 0) {
    std::cerr << "Connection " << sockfd << " is closed." << std::endl;
    return -1;
  }

  if ((numbytes = recv(sockfd, &buf_type, 4, 0)) <= 0) {
    std::cerr << "Connection " << sockfd << " is closed." << std::endl;
    return -1;
  }
  
  size = buf_size;
  type = buf_type;
  char * buf = new char[buf_size];  

  //fdToSize[new_fd] = buf_size;
  //fdToType[new_fd] = buf_type;
  //delete [] fdToBuf[new_fd];
  //fdToBuf[new_fd] = new char[buf_size];
  numbytes = recv(sockfd, buf, buf_size, 0);
  msg = buf;
  delete [] buf;

  if (numbytes <= 0) {
    std::cerr << "Connection " << sockfd <<" is closed." << std::endl;
    //delete [] fdToBuf[new_fd];
    return -1;
  }

  return buf_size;
}

int socketSendMsg(int sockfd, int type, const Message & msg) {
  int buf_size = sizeof msg;
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
    std::cerr << "Sending msg failed." << std::endl;
    result = -1;
  }

  if (result >=0 && (numbytes = send(sockfd, msg_ser, buf_size, 0)) == -1) {
    result = -1;
    std::cerr << "Sending msg failed." << std::endl;
  }
  delete [] buf_ints;
  delete [] msg_ser;
  return result;


}


int socketRecvMsg(int sockfd, int & size, int & type, Message & msg) {
  int numbytes;

  int *buf_ints = new int[2];
  if ((numbytes = recv(sockfd, buf_ints, 8, 0)) <= 0) {
    std::cerr << "Connection " << sockfd << " is closed." << std::endl;
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
    std::cerr << "Connection " << sockfd <<" is closed." << std::endl;
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


 
