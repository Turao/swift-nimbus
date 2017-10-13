#pragma once

#include <string>
#include <vector>

#include "NimbusFile.h"

class User
{
public:
  User();
  ~User();

  void setUsername(std::string name);
  std::string getUsername();

  bool isLoggedIn();

protected:
private:
  std::string username;
  std::vector<int> devices;
  std::vector<NimbusFile> files;
  bool loggedIn;
};