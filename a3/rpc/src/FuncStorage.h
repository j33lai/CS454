#pragma once
#include "Utils.h"

#include <iostream>
#include <vector>
#include <map>
#include <utility>

class FuncStorage {
public:
  FuncStorage(std::string name, std::vector<int> types);

  int addServer(std::string server_name, int server_port);
  void updateServers(std::vector<std::pair<std::string, int>> servers); 

  void removeServer(int id);
  void removeServer(std::string server_name, int server_port);
  void setSkeleton(skeleton f);

  std::vector<std::pair<std::string, int>> getServers();
  std::pair<std::string, int> getServer(int i);
  int getNumOfServers();
  int hasServer(std::string server_name, int server_port);

  //std::string getServerName();
  //int getServerPort();

  bool equalToFunc(std::string name, std::vector<int> types); 
  int findInDb(std::map<std::string, std::vector<FuncStorage>> & db);
 
  std::string fName;
  std::vector<int> fArgTypes;
  std::vector<std::pair<std::string, int>> fServers;

  //std::vector<std::string> fServerNames;
  //std::vector<int> fServerPorts; 
  skeleton fSkeleton;  
};
