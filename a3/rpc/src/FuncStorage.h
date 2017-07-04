#pragma once
#include "Utils.h"

#include <iostream>
#include <vector>
#include <map>

class FuncStorage {
public:
  FuncStorage(std::string name, std::vector<int> types);
  void addServer(std::string server_name, int server_port);
  void removeServer(int id);
  void setSkeleton(skeleton f);

  std::string getServerName();
  int getServerPort();

  bool equalToFunc(std::string name, std::vector<int> types); 
  int findInDb(std::map<std::string, std::vector<FuncStorage>> & db);
 
  std::string fName;
  std::vector<int> fArgTypes;
  std::vector<std::string> fServerNames;
  std::vector<int> fServerPorts; 
  skeleton fSkeleton;  
};
