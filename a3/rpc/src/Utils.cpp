#include "Utils.h"
#include <sstream>

std::vector<std::string> split(std::string str, char delimiter) {
  std::vector<std::string> internal;
  std::stringstream ss(str); // Turn the string into a stream.
  std::string tok;
  
  while(getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }
  
  return internal;
}

int getArgType(int type) {
  return (type << 8) >> 24;
}

int getArgSize(int type) {
  int size = (type << 16) >> 16;
  return size <= 0 ? 1 : size;
}

int getTypeSize(int type) {
  int argType = getArgType(type);
  int argSize = getArgSize(type);
  switch(argType) {
    case ARG_CHAR:
    case ARG_SHORT:
      return argType * argSize;
    case ARG_INT:
    case ARG_FLOAT:
      return 4 * argSize;
    case ARG_LONG:
    case ARG_DOUBLE:
      return 8 * argSize;
    default:
      return 0;
  } 
}

std::vector<int> reorderArgTypes(std::vector<int> & types) {  // input, output, 0
  std::vector<int> result;
  std::vector<int> output;
  int types_size = types.size();
  for (int i = 0; i < types_size - 1; i++) {
    int t = (types[i] >> 16) << 16;   // eliminate array size for comparison
    if (t < 0) {
      result.push_back(t);
    } else {
      output.push_back(t);
    }
  }
  result.insert(result.begin(), output.begin(), output.end());
  result.push_back(0);
  return result;
}
