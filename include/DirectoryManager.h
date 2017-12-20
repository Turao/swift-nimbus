#pragma once

#include <string>
#include <vector>
#include <dirent.h>
#include <thread>
#include <atomic>
#include <mutex>

#include "Session.h"
#include "NimbusFile.h"

class DirectoryManager
{
public:
  DirectoryManager(std::string username, Session *session);
  ~DirectoryManager();

  std::string getPath();
  std::vector<NimbusFile*> getFiles();
  
  void printDirectoryEntries();
  
  void lockFiles();
  void unlockFiles();

protected:

private: 
  std::string path;
  DIR *directory;
  std::vector<NimbusFile*> files;

  Session *session; // used to send new files to the other endpoint
  

  std::atomic<bool> _checkForNewFiles_isRunning;
  std::thread _checkForNewFilesThread;
  void checkForNewFiles();

  std::mutex files_mtx;

  std::atomic<bool> _checkForRecentlyModifiedFiles_isRunning;
  std::thread _checkForRecentlyModifiedFilesThread;
  void checkForRecentlyModifiedFiles();

  std::atomic<bool> _checkForDeletedFiles_isRunning;
  std::thread _checkForDeletedFilesThread;
  void checkForDeletedFiles();
};
