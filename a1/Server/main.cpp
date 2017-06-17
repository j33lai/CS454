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

#define BACKLOG 10 

std::map<int, int> fdToSize;
std::map<int, char*> fdToBuf;
std::map<int, int> fdToRecv;

void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int sendAll(int sockfd, char* buf, int buf_size) {
  int sent = 0;
  int n;
  while (sent < buf_size) {
    if ((n = send(sockfd, buf+sent, buf_size-sent, 0)) == -1) {
      std::cerr << "Sending msg failed." << std::endl;
      return -1;
    } else {
      sent += n;
    }

  }
  return sent;
}

void sendToClient(int new_fd, char* buf) {
  int numbytes;
  std::cout << "String received from client: " << buf << std::endl;
  int i = 0;
  while(buf[i] != '\0' && i < fdToSize[new_fd]) {
    if ((i - 1 >= 0 && buf[i-1] == ' ') || i == 0) {
      if (buf[i] >= 97 && buf[i] <=122) {
        buf[i] -= 32;
      } 
    } else {
      if (buf[i] >= 65 && buf[i] <=90) {
        buf[i] += 32;
      }
    }
    i++;
  }

  if ((numbytes = sendAll(new_fd, fdToBuf[new_fd], fdToSize[new_fd])) == -1) {
    std::cout << "Sending to client fails." << std::endl;
  } 
}

int dealWithClient(int size, int new_fd) {
  int numbytes;
  if (size <= 0) {
    int buf_size;
    if ((numbytes = recv(new_fd, &buf_size, 4, 0)) <= 0) {
      std::cerr << "Connection " << new_fd << " is closed." << std::endl;
      return -1;
    } else {
      fdToBuf[new_fd] = new char[buf_size];
      fdToRecv[new_fd] = 0;
      return buf_size;
    }
  } else {
    //char buf[size]; 
    numbytes = recv(new_fd, fdToBuf[new_fd] + fdToRecv[new_fd], size - fdToRecv[new_fd], 0);
    if (numbytes <= 0) {
      std::cerr << "Connection " << new_fd <<" is closed." << std::endl;
      delete [] fdToBuf[new_fd];
      fdToRecv.erase(new_fd);
      return -1;
    } else {
      fdToRecv[new_fd] += numbytes;       
    } 

    if (size - fdToRecv[new_fd] == 0) {
      sendToClient(new_fd, fdToBuf[new_fd]);      

      delete [] fdToBuf[new_fd];
      fdToRecv[new_fd] = 0;

    } else {
      return size;
    }
  }

  return 0;
}


int main() {
  std::cout << "I am the server!" << std::endl;
  
  int status;
  int yes = 1;
  int sockfd, new_fd;
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr;
  socklen_t sin_size;
  char ipstr[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof hints); // make sure the struct is empty
  hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
  hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

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
      socklen_t len;
      struct sockaddr_storage addr;

      getsockname(sockfd, (struct sockaddr*)&addr, &len);
      //getpeername(sockfd, (struct sockaddr*)&addr, &len);

      int port;
      
      if (addr.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&addr;
        port = ntohs(s->sin_port);
        //inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
      } else {
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
        port = ntohs(s->sin6_port);
        //inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
      }

      char hostname[128];
      gethostname(hostname, sizeof hostname);

      std::cout << "SERVER_ADDRESS " << hostname << std::endl;
      std::cout << "SERVER_PORT " << port << std::endl;


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


  fd_set master;
  fd_set read_fds;
  int fdmax = sockfd;
  FD_ZERO(&master);
  FD_ZERO(&read_fds);
  FD_SET(sockfd, &master);
  
  //std::cout << fdmax << std::endl;  


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
          int size = dealWithClient(fdToSize[i], i);
          if (size < 0) {
            fdToSize.erase(i);
            fdToRecv.erase(i);
            close(i);
            FD_CLR(i, &master); 
          } else {
            fdToSize[i] = size;
          }
        }
      } 
    }    
 
  } 
  close(sockfd);
  return 0;
}

