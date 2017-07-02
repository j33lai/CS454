#pragma once
#include <string>
#include <vector>

#define MSG_BINDER_CLIENT 0
#define MSG_BINDER_SERVER 1
#define MSG_CLIENT_SERVER 2

#define REGISTER 10
#define REGISTER_SUCCESS 11
#define REGISTER_FAILURE 12
#define LOC_REQUEST 13
#define LOC_SUCCESS 14
#define LOC_FAILURE 15
#define EXECUTE 16
#define EXECUTE_SUCCESS 17
#define EXECUTE_FAILURE 18
#define TERMINATE 19

#define HOST_NAME_SIZE 128
#define FUNCTION_NAME_SIZE 64

typedef int (*skeleton)(int *, void **);

std::vector<std::string> split(std::string str, char delimiter);

