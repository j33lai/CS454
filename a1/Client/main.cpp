#include <iostream>
#include <pthread.h>
#include <thread>
#include <chrono>
#include <string>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAXDATASIZE 100

std::vector<std::string> msg;
pthread_mutex_t mutexmsg;
int sockfd;
int end_of_file = 0;
void *SendToServer(void *tmpmsg);

void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void *PrintHello(void *threadid) {
  int signal = 0;
  while(true) {
    std::string tmp = "";
    pthread_mutex_lock(&mutexmsg);
    if (msg.size() > 0) {
      tmp = msg[0];
      msg.erase(msg.begin());
    }
    signal = end_of_file;
    pthread_mutex_unlock(&mutexmsg);
    
    if (tmp.length() > 0) {
      pthread_t thread_send_msg_to_server;
      int rc_send;
      rc_send = pthread_create(&thread_send_msg_to_server, NULL, SendToServer, &tmp);
      if (rc_send) {
        std::cout << "ERROR: " << "fail to create sending thread." << std::endl;
      } else {
        std::this_thread::sleep_for((std::chrono::seconds(2)));
      }
    } else if (signal > 0) {
      break;
    }
  }
  
  close(sockfd);
  //long tid;
  //tid = (long)threadid;
  //printf("Hello World! It's me, thread #%ld!\n", tid);
  pthread_exit(NULL);
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

int recvAll(int sockfd, char* buf, int buf_size) {
  int received = 0;
  int n;
  while (received < buf_size) {
    if ((n = recv(sockfd, buf+received, buf_size-received, 0)) == -1) {
      std::cerr << "Receiving msg failed." << std::endl;
      return -1;
    } else {
      received += n;
    }

  }
  return received;
}

void *SendToServer(void *tmpmsg) {
  int n;
  std::string tmp = *reinterpret_cast<std::string*>(tmpmsg);
  char *buf = new char[tmp.length() + 1];
  int buf_size = tmp.length() + 1;
  buf = strcpy(buf, tmp.c_str());
  buf[tmp.length()] = '\0';

  if ((n = send(sockfd, &buf_size, 4, 0)) == -1) {
    std::cerr << "Sending msg failed." << std::endl;
  } else {
    //if ((n = send(sockfd, buf, buf_size, 0)) == -1) {
    if ((n = sendAll(sockfd, buf, buf_size)) == -1) {
      std::cerr << "Sending msg failed." << std::endl;
    } else {
      //if ((n = recv(sockfd, buf, buf_size, 0)) <= 0) {
      if ((n = recvAll(sockfd, buf, buf_size)) <= 0) {
        std::cerr << "Receiving msg failed." << std::endl;
      } else {
        std::cout << "Server: " << buf << std::endl;
      }
    }
  }
  delete [] buf;
  pthread_exit(NULL);
}

int main() {
  std::cout << "I am the client!" << std::endl;
  //int sockfd, numbytes;  
  //char buf[MAXDATASIZE];
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];

  char* server_address;
  char* server_port;
  server_address = getenv("SEVER_ADDRESS");
  server_port = getenv("SEVER_PORT");
  if (server_address != nullptr && server_port != nullptr) {
    //std::cout << server_address << std::endl;
    //std::cout << server_port << std::endl;
  } else {
    std::cout << "Server address or port not found!" << std::endl;
    return 1;
  }

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(server_address, server_port, &hints, &servinfo)) != 0) {
    std::cerr << "getaddrinfo: " << gai_strerror(rv) << std::endl;
    return 1;
  }

  for(p = servinfo; p != nullptr; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      std::cerr << "client: socket" << std::endl;  
      continue;
    }

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      std::cerr << "client: connect" << std::endl;  
      continue;
    }

    break;
  }

  if (p == nullptr) {
    std::cerr << "client: failed to connect" << std::endl;
    return 2;
  }

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);

  freeaddrinfo(servinfo);
  /*
  if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
    std::cerr << "recv" << std::endl;
    exit(1);
  }

  buf[numbytes] = '\0';

  std::cout << "client: received " << buf << std::endl;
  close(sockfd);
  */

  pthread_mutex_init(&mutexmsg, NULL);

  pthread_t test;
  int rc;
  long t = 0;
  rc = pthread_create(&test, NULL, PrintHello, (void *)t);
  if (rc) {
    std::cout << "ERROR: " << rc << std::endl;
  }

  std::string i;
  while (true) {
    //std::cout << " ";
    //std::cin >> i;
    if (std::cin.eof()==1) {
      break;
    }
    getline(std::cin, i); 
    pthread_mutex_lock(&mutexmsg);
    msg.push_back(i);
    pthread_mutex_unlock(&mutexmsg);
  }

  pthread_mutex_lock(&mutexmsg);
  end_of_file = 1;
  pthread_mutex_unlock(&mutexmsg);

  pthread_mutex_destroy(&mutexmsg);
  pthread_exit(NULL);
}
