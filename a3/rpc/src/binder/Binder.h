#pragma once
#include "../Message.h"
#include "../FuncStorage.h"
#include "../FuncDatabase.h"

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
 
  int sockfd;
  std::map<int, int> fdToSize;
  std::map<int, int> fdToType;
  std::map<int, Message> fdToMsg;

  FuncDatabase funcDatabase;  // database for func and server info

  bool terminating = false; // indicate whether or not the binder is terminating
};
