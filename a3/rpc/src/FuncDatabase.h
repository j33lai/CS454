#pragma once
#include "FuncStorage.h"

#include <iostream>
#include <vector>
#include <map>
#include <utility>

class FuncDatabase {
public:
  
  FuncStorage getFunc(std::string func_name, int func_id);
  std::vector<std::pair<std::string, int>> getServerList();

  int findFunc(std::string func_name, std::vector<int> & arg_types); 
  int addFunc(
    std::string func_name, 
    std::vector<int> & arg_types, 
    std::string server_name, 
    int server_port
  );
  
  int addFunc(
    std::string func_name,
    std::vector<int> & arg_types,
    std::vector<std::pair<std::string, int>> server_list 
  );
  

  void removeServer(std::string server_name, int server_port); 
 

  std::pair<std::string, int> getServerRR(std::string func_name, int func_id);

 
  int rr_id = -1;  // round robin index
  std::vector<std::pair<std::string, int>> serverList;  // func name to func storage
  std::map<std::string, std::vector<FuncStorage>> funcList;  // a list of servers registered 

private:
  void addServerToList(std::string server_name, int server_port);
  void addServersToList(std::vector<std::pair<std::string, int>> servers);
  void removeServerFromList(std::string server_name, int server_port);
};
