#include <iostream>

extern "C" int rpcCacheCall(const char* name, int* argTypes, void** args) {
  std::cout << "rpcCacheCall" << std::endl;
  return 0;
}