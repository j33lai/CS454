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
  Message(int type, std::string func_name, int *arg_types, void **args);
  Message(int type, std::string server_id, int server_port);
  Message(int type, std::string server_id, int server_port, std::string func_name, int *arg_types);  

  ~Message();
 
  void deleteArgs();  

  void initArgTypes(int *arg_types);
  int *getArgTypesPointer();
  int getSerializationSize() const;
  void setArgs(void **args);

  int mType;
  int reasonCode;
  char serverId[HOST_NAME_SIZE];
  int serverPort;
  char funcName[FUNCTION_NAME_SIZE];
  std::vector<int> argTypes;
  void **mArgs = nullptr;

};

void serializeMessage(Message *msg, char *msg_serialized);
void deserializeMessage(char *msg_serialized, Message *msg);

void serializeArg(void * & sp, int type, void *arg);
void deserializeArg(void * & sp, int type, void * & arg);

template <class T>
void serializeVariable(void * & sp, T *v, int size) {
  T *s = (T *)sp;
  for (int i = 0; i < size; i++) {
    *s = v[i];
    s++;
  }
  sp = (void *)s;
}

template <class T>
void deserializeVariable(void * & sp, T * & v, int size) {
  T *s = (T *)sp;
  v = new T[size];
  for (int i = 0; i < size; i++) {
    v[i] = *s;
    s++;
  }
  sp = (void *)s;
}


