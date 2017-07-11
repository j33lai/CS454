#pragma once
#include "Message.h"

#include <string>
#define BACKLOG 10

int socketInit(int & sockfd, std::string & host, int & port);
int socketConnect(int & sockfd, std::string destHost, std::string destPort);
void socketClose(int sockfd);
int socketSendAll(int sockfd, char* buf, int buf_size);
int socketRecvAll(int sockfd, char* buf, int buf_size);

int socketSendMsg(int sockfd, int type, const Message & msg);
int socketRecvMsg(int sockfd, int & size, int & type, Message & msg);
void *get_in_addr(struct sockaddr *sa); 
