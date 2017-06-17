#include <iostream>
#include "RPCLibrary.h"

int rpcCall(const char* name, int* argTypes, void** args) {
  std::cout << "rpcCall" << std::endl;
  return 1;
}

