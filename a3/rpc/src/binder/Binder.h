#pragma once
#include <map>
#include <string>

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

  int sockfd;
  std::map<int, int> fdToSize;
  std::map<int, int> fdToType;
  std::map<int, char*> fdToBuf;
  std::map<int, int> fdToRecv;
  std::map<std::string, std::string> binderDb;

};
