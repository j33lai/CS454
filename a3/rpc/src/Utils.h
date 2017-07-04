#pragma once
#include <string>
#include <vector>

#define ARG_CHAR    1
#define ARG_SHORT   2
#define ARG_INT     3
#define ARG_LONG    4
#define ARG_DOUBLE  5
#define ARG_FLOAT   6

#define ARG_INPUT   31
#define ARG_OUTPUT  30

#define MSG_BINDER_CLIENT 0
#define MSG_BINDER_SERVER 1
#define MSG_CLIENT_SERVER 2

#define REGISTER 10
#define REGISTER_SUCCESS 15
#define REGISTER_FAILURE 16
#define LOC_REQUEST 11
#define LOC_SUCCESS 12
#define LOC_FAILURE 17
#define EXECUTE 13
#define EXECUTE_SUCCESS 14
#define EXECUTE_FAILURE 18
#define TERMINATE 19

#define HOST_NAME_SIZE 128
#define FUNCTION_NAME_SIZE 64

typedef int (*skeleton)(int *, void **);

std::vector<std::string> split(std::string str, char delimiter);
int getArgType(int type);
int getArgSize(int type);
int getTypeSize(int type);
std::vector<int> reorderArgTypes(std::vector<int> & types);
