#include "../Utils.h"
#include <iostream>

extern "C" int rpcRegister(const char* name, int* argTypes, skeleton f) {
  std::cout << "rpcRegister" << std::endl;
  return 4;
}

