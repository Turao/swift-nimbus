#pragma once

#include <string>
#include <vector>

#include "NimbusFile.h"

class User
{
public:
  User(std::string name);
  ~User();

  std::string getName();
  
  void setLoggedIn(bool status);
  bool isLoggedIn();

protected:
private:
  std::string name;
  std::vector<int> devices;
  std::vector<NimbusFile> files;
  bool loggedIn;
};