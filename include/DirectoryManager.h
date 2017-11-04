#pragma once

#include <string>
#include <vector>

#include "NimbusFile.h"

class DirectoryManager
{
public:
  DirectoryManager(std::string username);
  ~DirectoryManager();

  void notify();
  std::vector<NimbusFile*> getFiles();

protected:

private: 
  std::string path;
  std::vector<NimbusFile*> files;
};
