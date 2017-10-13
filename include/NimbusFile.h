#pragma once

#include <string>

class NimbusFile
{
public:
  NimbusFile();
  ~NimbusFile();

protected:

private: 
  std::string name;
  std::string extension;
  std::string last_modified;
  int size;
};
