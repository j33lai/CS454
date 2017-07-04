#pragma once
#include "../Message.h"
#include "../FuncStorage.h"
#include <string>
#include <map>
#include <vector>

extern int binderSockfd;  // socket for connecting binder on the server
extern int clientsSockfd; // socket for accepting clients on the server
extern int clientsPort;
extern std::string serverAddr;
extern std::map<std::string, std::vector<FuncStorage>> serverDb;

extern std::map<int, int> server_fdToSize;
extern std::map<int, int> server_fdToType;
extern std::map<int, char*> server_fdToBuf;
extern std::map<int, Message> server_fdToMsg;
