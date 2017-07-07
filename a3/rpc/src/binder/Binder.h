#pragma once
#include "../Message.h"
#include "../FuncStorage.h"

#include <map>
#include <vector>
#include <utility>

class Binder {
public:
  Binder();
  
  void binderInit();
  void binderAccept();
  void binderTerminate();

private:
  int dealWith(int new_fd);
  void handle(int new_fd, bool connected);
  void handleClient(int new_fd, bool connected);
  void handleServer(int new_fd, bool connected);
  void handleTermination();

  int hasFunc(std::string name, std::vector<int> argTypes);
  int addFunc(const Message & msg);

  void addServer(std::string name, int port);
  void removeServer(std::string name, int port);
  void removeServer(int fd);

  std::pair<std::string, int> getServerRR(std::string func_name, int func_id);

  int sockfd;
  std::map<int, int> fdToSize;
  std::map<int, int> fdToType;
  std::map<int, char*> fdToBuf;
  std::map<int, Message> fdToMsg;

  std::map<int, int> fdToRecv;

  std::map<std::string, std::vector<FuncStorage>> binderFuncs;  // func name to func storage

  std::vector<std::pair<std::string, int>> serverList;  // a list of servers registered with the binder

  int rr_id = -1;  //  round robin index

  bool terminating = false; // indicate whether or not the binder is terminating
  

};
