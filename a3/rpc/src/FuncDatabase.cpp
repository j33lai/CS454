#include "FuncDatabase.h"

FuncStorage FuncDatabase::getFunc(std::string func_name, int func_id) {
  return funcList[func_name][func_id];
}

std::vector<std::pair<std::string, int>> FuncDatabase::getServerList() {
  return serverList;
}

int FuncDatabase::findFunc(std::string func_name, std::vector<int> & arg_types) {
  if (funcList.find(func_name) != funcList.end()) {
     for (unsigned i = 0; i < funcList[func_name].size(); i++) {
       if (funcList[func_name][i].equalToFunc(func_name, arg_types)) {
          return i;
       }
     }
  }
  return -1;
}

int FuncDatabase::addFunc(
  std::string func_name, 
  std::vector<int> & arg_types, 
  std::string server_name, 
  int server_port
) {
  int result = 0;
  try { 
    int func_id = findFunc(func_name, arg_types);
    if (func_id < 0) {
      FuncStorage funcStorage(func_name, arg_types);
      funcStorage.addServer(server_name, server_port);
      funcList[func_name].push_back(funcStorage);
    } else {
      if (funcList[func_name][func_id].addServer(server_name, server_port) > 0) {
        result = 1;  // server is already added
      }
    }
    if (result == 0) {
      addServerToList(server_name, server_port);  // add server to the server list
    }
  } catch (...) { 
    result = -1;
  }  
  return result;
}

int FuncDatabase::addFunc(
  std::string func_name, 
  std::vector<int> & arg_types,
  std::vector<std::pair<std::string, int>> server_list
) {
  int result = 0;
  try { 
    int func_id = findFunc(func_name, arg_types);
    if (func_id < 0) {
      FuncStorage funcStorage(func_name, arg_types);
      funcStorage.updateServers(server_list);
      funcList[func_name].push_back(funcStorage);
    } else {
      funcList[func_name][func_id].updateServers(server_list); 
    }
    addServersToList(server_list);  // add servers to the server list
  } catch (...) {
    result = -1;
  }
  return result;
}

void FuncDatabase::removeServer(std::string server_name, int server_port) {
  for (auto & vs : funcList) {
    unsigned i = 0;
    while (i < vs.second.size()) {
      vs.second[i].removeServer(server_name, server_port);
      if (vs.second[i].getNumOfServers() == 0) {
        // remove the FuncStorage if no servers are registered for it 
        vs.second.erase(vs.second.begin() + i);
      } else {
        i++;
      }
    }
  }
  removeServerFromList(server_name, server_port);
}

std::pair<std::string, int> FuncDatabase::getServerRR(std::string func_name, int func_id) {
  FuncStorage funcStorage = getFunc(func_name, func_id);
  int rr_size = serverList.size();
  int i = (rr_id + 1) % rr_size;
  while (true) {
    int id = funcStorage.hasServer(serverList[i].first, serverList[i].second);
    if (id >= 0) {
      rr_id = i;
      return funcStorage.getServer(id);
    }
    i = (i + 1) % rr_size;
  }
}


void FuncDatabase::addServerToList(std::string server_name, int server_port) {
  for (unsigned i = 0; i < serverList.size(); i++) {
    if (serverList[i].first == server_name && serverList[i].second == server_port) {
      return;  // server is already registered
    }
  }
  // add server
  std::cout << "add server" << std::endl;
  std::pair<std::string, int> new_server(server_name, server_port);
  serverList.push_back(new_server);
}

void FuncDatabase::addServersToList(std::vector<std::pair<std::string, int>> servers) {
  for (unsigned i = 0; i < servers.size(); i++) {
    addServerToList(servers[i].first, servers[i].second);
  }
}

void FuncDatabase::removeServerFromList(std::string server_name, int server_port) {
  // remover server from the server list
  int rr_size = serverList.size();
  for (int i = 0; i < rr_size; i++) {
    if (serverList[i].first == server_name && serverList[i].second == server_port) {
      if (i <= rr_id) {
        rr_id -= 1;
      }
      serverList.erase(serverList.begin() + i);
      break;
    }
  }
}


