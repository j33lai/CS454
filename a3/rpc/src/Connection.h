#pragma once

#include <string>
#define BACKLOG 10

void socketInit(int & sockfd, std::string & host, int & port);
void socketClose(int sockfd);
void *get_in_addr(struct sockaddr *sa); 
