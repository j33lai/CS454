#pragma once

#include <string>
#define BACKLOG 10

void socketInit(int & sockfd, std::string & host, int & port);
void socketConnect(int & sockfd, std::string destHost, std::string destPort);
void socketClose(int sockfd);
void *get_in_addr(struct sockaddr *sa); 
