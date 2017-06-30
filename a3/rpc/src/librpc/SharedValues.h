#pragma once
#include <string>
#include <map>

extern int binderSockfd;  // socket for connecting binder on the server
extern int clientsSockfd; // socket for accepting clients on the server
extern int clientsPort;
extern std::string serverAddr;
extern std::map<std::string, std::string> serverDb;

extern std::map<int, int> server_fdToSize;
extern std::map<int, int> server_fdToType;
extern std::map<int, char*> server_fdToBuf;
