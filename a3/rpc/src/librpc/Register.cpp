#include "SharedValues.h"
#include "../Utils.h"

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

std::map<std::string, std::string> serverDb;

extern "C" int rpcRegister(const char* name, int* argTypes, skeleton f) {
  std::cout << "rpcRegister" << std::endl;

  std::string tmp_name = name;
  tmp_name += " " + serverAddr + " " + std::to_string(clientsPort);
  std::cout << "server accept port " << clientsPort << std::endl;


  int buf_size = tmp_name.length() + 1;
  int buf_type = MSG_BINDER_SERVER;
  char *buf = new char[tmp_name.length() + 1];
  buf = strcpy(buf, tmp_name.c_str());
  
  int result = 0;
  int numbytes;

  if ((numbytes = send(binderSockfd, &buf_size, 4, 0)) == -1) { 
    std::cerr << "Sending msg failed." << std::endl;
    result = -1;
  }

  if (result >=0 && (numbytes = send(binderSockfd, &buf_type, 4, 0)) == -1) {           
    std::cerr << "Sending msg failed." << std::endl;
  } 

  if (result >=0 && (numbytes = send(binderSockfd, buf, buf_size, 0)) == -1) { 
    std::cerr << "Sending msg failed." << std::endl;
  }

  serverDb[tmp_name]  = tmp_name + " " + "test";

  delete [] buf;

  return result;
}

