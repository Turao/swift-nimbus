#pragma once

#include <string>
#include <vector>

#include "NimbusFile.h"

class DirectoryManager
{
public:
  DirectoryManager();
  ~DirectoryManager();

  void notify();

protected:

private: 
  std::vector<NimbusFile> directoryFiles;
};
