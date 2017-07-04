#pragma once
#include "../Message.h"
#include "../FuncStorage.h"

#include <map>
#include <vector>

class Binder {
public:
  Binder();
  
  void binderInit();
  void binderAccept();
  void binderTerminate();

private:
  int dealWith(int new_fd);
  void handle(int new_fd);
  void handleClient(int new_fd);
  void handleServer(int new_fd);
  int hasFunc(std::string name, std::vector<int> argTypes);

  int sockfd;
  std::map<int, int> fdToSize;
  std::map<int, int> fdToType;
  std::map<int, char*> fdToBuf;
  std::map<int, Message> fdToMsg;

  std::map<int, int> fdToRecv;

  std::map<std::string, std::vector<FuncStorage>> binderFuncs;  // func name to func storage

  //std::map<std::string, std::vector<int>> binderDb2;
};
