#pragma once
#include <map>

class Binder {
public:
  Binder();
  
  void binderInit();
  void binderAccept();
  void binderTerminate();

private:
  int dealWith(int new_fd);
  void handleClient();
  void handleServer();

  int sockfd;
  std::map<int, int> fdToSize;
  std::map<int, int> fdToType;
  std::map<int, char*> fdToBuf;
  std::map<int, int> fdToRecv;

};
