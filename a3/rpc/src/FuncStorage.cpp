#include "FuncStorage.h"

FuncStorage::FuncStorage(std::string name, std::vector<int> types):
  fName(name), fArgTypes(types) {
}

int FuncStorage::addServer(std::string server_name, int server_port) {
  for (unsigned i = 0; i < fServers.size(); i++) {
    if (server_name == fServers[i].first && server_port == fServers[i].second) {
      return 1; // duplicated registration
    }
  }

  std::pair<std::string, int> new_server(server_name, server_port);
  fServers.push_back(new_server);
  return 0;
}

void FuncStorage::removeServer(int id) {
  fServers.erase(fServers.begin() + id);
}

void FuncStorage::removeServer(std::string server_name, int server_port) {
  unsigned i = 0;
  while (i < fServers.size()) {
    if (server_name == fServers[i].first && server_port == fServers[i].second) {
      fServers.erase(fServers.begin() + i);
    } else {
      i++;
    }
  }  
}

void FuncStorage::setSkeleton(skeleton f) {
  fSkeleton = f;
}

std::vector<std::pair<std::string, int>> FuncStorage::getServers() {
  return fServers;
}

std::pair<std::string, int> FuncStorage::getServer(int i) {
  return fServers[i];
}

int FuncStorage::hasServer(std::string server_name, int server_port) {
  for (unsigned i = 0; i < fServers.size(); i++) {
    if (server_name == fServers[i].first && server_port == fServers[i].second) {
      return i;
    }
  }
  return -1;
}


/*
std::string FuncStorage::getServerName() {
  if (fServerNames.size() > 0) {
    return fServerNames[0];
  } else {
    return "";
  }
}

int FuncStorage::getServerPort() {
  if (fServerPorts.size() > 0) {
    return fServerPorts[0];
  } else {
    return 0;
  }
}
*/

bool FuncStorage::equalToFunc(std::string name, std::vector<int> types) {
  if (name != fName) {
    return false;
  }

  if (types.size() != fArgTypes.size()) {
    return false;
  }

  std::vector<int> t0 = reorderArgTypes(fArgTypes);
  std::vector<int> t1 = reorderArgTypes(types);

  int t_size = t0.size();
  for (int i = 0; i < t_size - 1; i++) {
    if (t0[i] != t1[i]) {
      return false;
    } 
  }

  return true;
}

int FuncStorage::findInDb(std::map<std::string, std::vector<FuncStorage>> & db) {
  if (db.find(fName) != db.end()) {
     int db_fName_size = db[fName].size();
     for (int i = 0; i < db_fName_size; i++) {
       if (db[fName][i].equalToFunc(fName, fArgTypes)) {
          return i;
       }
     }
  }
  return -1;
}


