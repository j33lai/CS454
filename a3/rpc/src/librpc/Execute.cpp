#include <iostream>

extern "C" int rpcExecute() {
  std::cout << "rpcExecute" << std::endl;
  return 2;
}

