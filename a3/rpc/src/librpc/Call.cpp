#include <iostream>

extern "C" int rpcCall(const char* name, int* argTypes, void** args) {
  std::cout << "rpcCall" << std::endl;
  return 1;
}

