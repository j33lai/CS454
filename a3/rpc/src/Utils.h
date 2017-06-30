#pragma once
#include <string>
#include <vector>

#define MSG_BINDER_CLIENT 0
#define MSG_BINDER_SERVER 1
#define MSG_CLIENT_SERVER 2

typedef int (*skeleton)(int *, void **);

std::vector<std::string> split(std::string str, char delimiter);

