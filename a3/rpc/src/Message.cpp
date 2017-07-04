#include "Message.h"
#include <cassert>
#include <string.h>

Message::Message() {}

Message::Message(int type) : mType(type) {
  assert(type == TERMINATE);
}

Message::Message(int type, int code):
  mType(type),
  reasonCode(code) {
  assert(type >= REGISTER_SUCCESS && type <= EXECUTE_FAILURE);
}

Message::Message(int type, std::string func_name, int *arg_types) :
  mType(type) {  
  assert(type == LOC_REQUEST);
  initArgTypes(arg_types);
  strcpy(funcName, func_name.c_str());
}

Message::Message(int type, std::string func_name, int *arg_types, void **args) :
  mType(type),
  mArgs(args) { 
  assert(type >= EXECUTE && type <= EXECUTE_SUCCESS);
  initArgTypes(arg_types);
  strcpy(funcName, func_name.c_str());
}


Message::Message(int type, std::string server_id, int server_port) :
  mType(type),
  serverPort(server_port) {
  assert(type == LOC_SUCCESS);
  strcpy(serverId, server_id.c_str());
}


Message::Message(int type, std::string server_id, int server_port, std::string func_name, int *arg_types) :
  mType(type),
  serverPort(server_port) {
  assert(type == REGISTER);
  initArgTypes(arg_types);
  strcpy(serverId, server_id.c_str());
  strcpy(funcName, func_name.c_str());
}

Message::~Message() {
// memory leak is handled manually, i.e. mArgs can be deleted later or should not be deleted
}

void Message::deleteArgs() {
  if (mArgs != nullptr) {
    int argTypes_size = argTypes.size();
    for (int i= 0; i < argTypes_size-1; i++) {
      if (mArgs[i] == nullptr) {
        continue;
      }
      int argType = getArgType(argTypes[i]);
      switch (argType) {
        case ARG_CHAR:
          delete [] (char*)mArgs[i];
          break;
        case ARG_SHORT:
          delete [] (short*)mArgs[i];
          break;
        case ARG_INT:
          delete [] (int*)mArgs[i]; 
          break;
        case ARG_LONG:
          delete [] (long*)mArgs[i];
          break;
        case ARG_DOUBLE:
          delete [] (double*)mArgs[i];
          break;
        case ARG_FLOAT:
          delete [] (float*)mArgs[i];
          break;
        default:
          break;
      }
    }
    delete [] mArgs;
  }
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

int *Message::getArgTypesPointer() {
  int *arg_types = new int[argTypes.size()];
  int argTypes_size = argTypes.size();
  for (int i = 0; i < argTypes_size; i++) {
    arg_types[i] = argTypes[i];
  }
  return arg_types;
}

int Message::getSerializationSize() const {
  int base_size = 204 + argTypes.size() * 4;
  if (mArgs != nullptr) {
    int args_size = 0;
    for (int t : argTypes) {
      args_size += getTypeSize(t);      
    }
    return base_size + args_size;    
  } else {
    return base_size;
  }

}

void Message::setArgs(void **args) {
  mArgs = args;
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

  if (msg->mArgs != nullptr) {
    void * s_a = (void*) s_is;
    int argTypes_size = msg->argTypes.size();
    for (int i = 0; i < argTypes_size-1; i++) {
      serializeArg(s_a, msg->argTypes[i], msg->mArgs[i]); 
    }
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
    msg->argTypes.push_back(*s_is);
    s_is++;
  }
  msg->argTypes.push_back(0);
  s_is++;

  if (msg->mType == EXECUTE || msg->mType == EXECUTE_SUCCESS) {
    if (msg->mArgs == nullptr) {
      msg->mArgs = new void*[msg->argTypes.size()-1]; 
    }
    void * s_a = (void*) s_is;
    int argTypes_size = msg->argTypes.size();
    for (int i = 0; i < argTypes_size-1; i++) {
      deserializeArg(s_a, msg->argTypes[i], msg->mArgs[i]);
    } 
  }
}

void serializeArg(void * & sp, int type, void *arg) {
  int argType = getArgType(type);
  int argSize = getArgSize(type);
  switch (argType) {
    case ARG_CHAR:
      serializeVariable(sp, (char *)arg, argSize);
      break;
    case ARG_SHORT:
      serializeVariable(sp, (short *)arg, argSize);
      break;
    case ARG_INT:
      serializeVariable(sp, (int *)arg, argSize);
      break;
    case ARG_LONG: 
      serializeVariable(sp, (long *)arg, argSize);
      break;
    case ARG_DOUBLE:
      serializeVariable(sp, (double *)arg, argSize);
      break;
    case ARG_FLOAT:
      serializeVariable(sp, (float *)arg, argSize);
      break;
    default:
      break;
  }
}

void deserializeArg(void * & sp, int type, void * & arg) {
  int argType = getArgType(type);
  int argSize = getArgSize(type);
  char *arg_char;
  short *arg_short;
  int *arg_int;
  long *arg_long;
  double *arg_double;
  float *arg_float;
  switch (argType) {
    case ARG_CHAR:
      arg_char = (char *)arg;
      deserializeVariable(sp, arg_char, argSize);
      arg = (void *)arg_char;
      break;
    case ARG_SHORT:
      arg_short = (short *)arg;
      deserializeVariable(sp, arg_short, argSize);
      arg = (void *)arg_short;
      break;
    case ARG_INT:
      arg_int = (int *)arg;
      deserializeVariable(sp, arg_int, argSize);
      arg = (void *)arg_int;
      break;
    case ARG_LONG:
      arg_long = (long *)arg;
      deserializeVariable(sp, arg_long, argSize);
      arg = (void *)arg_long;
      break;
    case ARG_DOUBLE:
      arg_double = (double *)arg;
      deserializeVariable(sp, arg_double, argSize);
      arg = (void *)arg_double;
      break;
    case ARG_FLOAT:
      arg_float = (float *)arg; 
      deserializeVariable(sp, arg_float, argSize);
      arg = (void *)arg_float;
      break;
    default:
      break;
  }
}



