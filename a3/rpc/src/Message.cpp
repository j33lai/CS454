#include "Message.h"
#include <string.h>

Message::Message() {}

Message::Message(int type) : mType(type) {}

Message::Message(int type, int code):
  mType(type),
  reasonCode(code) {
}

void Message::initArgTypes(int *arg_types) {
  int *ptr = arg_types;
  int i = 0;  
  while (ptr != nullptr && *ptr != 0) {
    argTypes.push_back(*ptr);
    ptr++;
    i++;
  }
  argTypes.push_back(0);
}

Message::Message(int type, std::string func_name, int *arg_types) :
  mType(type) {  
  strcpy(funcName, func_name.c_str());
  initArgTypes(arg_types);
}

Message::Message(int type, std::string server_id, int server_port) :
  mType(type),
  serverPort(server_port) {
  strcpy(serverId, server_id.c_str());
}

Message::Message(int type, std::string server_id, int server_port, std::string func_name, int *arg_types) :
  mType(type),
  serverPort(server_port) {
  strcpy(serverId, server_id.c_str());
  strcpy(funcName, func_name.c_str());
  initArgTypes(arg_types);
}

void serializeMessage(Message *msg, char *msg_serialized) {
  int *s_i = (int*)msg_serialized;
  *s_i = msg->mType;
  s_i++;
  *s_i = msg->reasonCode;
  s_i++;
  *s_i = msg->serverPort;
  s_i++;

  char *s_c = (char*)s_i;
  for (int i = 0; i < HOST_NAME_SIZE; i++) {
    *s_c = msg->serverId[i];
    s_c++;
  }

  for (int i = 0; i < FUNCTION_NAME_SIZE; i++) {
    *s_c = msg->funcName[i];
    s_c++;
  }
 
  int *s_is = (int*)s_c;
  for (int i : msg->argTypes) {
    *s_is = i;
    s_is++;
  }
}

void deserializeMessage(char *msg_serialized, Message *msg) {
  int *s_i = (int*)msg_serialized;
  msg->mType = *s_i;
  s_i++;
  msg->reasonCode = *s_i;
  s_i++;
  msg->serverPort = *s_i;
  s_i++;

  char *s_c = (char*)s_i;
  for (int i = 0; i < HOST_NAME_SIZE; i++) {
    msg->serverId[i] = *s_c;
    s_c++;
  }

  for (int i = 0; i < FUNCTION_NAME_SIZE; i++) {
    msg->funcName[i] = *s_c;
    s_c++;
  }

  int *s_is = (int*)s_c;
  while(*s_is != 0) {
    std::cout << *s_is << std::endl;
    msg->argTypes.push_back(*s_is);
    s_is++;
  }
  msg->argTypes.push_back(0);
}
