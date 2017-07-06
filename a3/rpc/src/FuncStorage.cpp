#include "FuncStorage.h"

FuncStorage::FuncStorage(std::string name, std::vector<int> types):
  fName(name), fArgTypes(types) {
}

int FuncStorage::addServer(std::string server_name, int server_port) {
  for (unsigned i = 0; i < fServerNames.size(); i++) {
    if (server_name == fServerNames[i] && server_port == fServerPorts[i]) {
      return 1; // duplicated registration
    }
  }

  fServerNames.push_back(server_name);
  fServerPorts.push_back(server_port);
  return 0;
}

void FuncStorage::removeServer(int id) {
  fServerNames.erase(fServerNames.begin() + id);
  fServerPorts.erase(fServerPorts.begin() + id);
}

void FuncStorage::removeServer(std::string s) {
  unsigned i = 0;
  while (i < fServerNames.size()) {
    if (s == fServerNames[i]) {
      fServerNames.erase(fServerNames.begin() + i);
      fServerPorts.erase(fServerPorts.begin() + i);
    } else {
      i++;
    }
  }  
}

void FuncStorage::setSkeleton(skeleton f) {
  fSkeleton = f;
}

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


