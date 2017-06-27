#include <iostream>

extern "C" int rpcTerminate() {
  std::cout << "rpcTerminate" << std::endl;
  return 5;
}

