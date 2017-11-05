#pragma once

#include <string>
#include <vector>
#include <dirent.h>
#include <thread>
#include <atomic>

#include "NimbusFile.h"

class DirectoryManager
{
public:
  DirectoryManager(std::string username);
  ~DirectoryManager();

  std::vector<NimbusFile*> getFiles();

protected:

private: 
  std::string path;
  DIR *directory;
  std::vector<NimbusFile*> files;
  

  std::atomic<bool> _checkForNewFiles_isRunning;
  std::thread _checkForNewFilesThread;
  void checkForNewFiles();
};
