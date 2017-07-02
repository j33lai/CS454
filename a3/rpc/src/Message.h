#pragma once
#include <iostream>
#include <vector>

#include "Utils.h"

class Message {
public:
  Message();
  Message(int type);
  Message(int type, int code);
  Message(int type, std::string func_name, int *arg_types);
  Message(int type, std::string server_id, int server_port);
  Message(int type, std::string server_id, int server_port, std::string func_name, int *arg_types);  

  void initArgTypes(int *arg_types);

  int mType;
  int reasonCode;
  char serverId[HOST_NAME_SIZE];
  int serverPort;
  char funcName[FUNCTION_NAME_SIZE];
  std::vector<int> argTypes;

};

void serializeMessage(Message *msg, char *msg_serialized);
void deserializeMessage(char *msg_serialized, Message *msg);
