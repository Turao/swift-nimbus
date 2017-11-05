#include "DirectoryManager.h"

#include <iostream>
#include <string>
#include <cstring>


#include <dirent.h> // directory opening and entries reading
#include <sys/stat.h> // to use mkdir

#include "NimbusFile.h"

#define DEBUG_PATH_PREFIX "/home/lenz/Desktop/sync_dir_" // to-do: remove
#define PATH_PREFIX "/home/sync_dir_"

DirectoryManager::DirectoryManager(std::string username, Session *session) :
_checkForNewFiles_isRunning(false),
directory(nullptr),
session(session)
{
  this->path = DEBUG_PATH_PREFIX + username;
  std::cout << "Directory Manager initialized at: " 
            << this->path 
            << std::endl;

  // check directory existence
  directory = opendir(this->path.c_str());
  if(directory == nullptr) {
    std::cout << "Error while opening sync directory: "
              << strerror(errno)
              << std::endl;

    if(errno == ENOENT) {
      // directory does not exist
      // and we must create it
      std::cout << "Creating sync dir at: "
              << this->path
              << std::endl;
      // create directory with the following mode:
      // "read, write, execute/search by owner"
      if(mkdir(this->path.c_str(), S_IRWXU) == -1) {
        std::cout << "Error while creating sync directory: "
                  << strerror(errno)
                  << std::endl;
        exit(1);
      }
      std::cout << "Sync dir created at: "
                << this->path
                << std::endl;
      // updates DIR entry pointer
      directory = opendir(this->path.c_str());
    }
    else {
      // any other errors should stop the execution
      closedir(directory);
      exit(1);
    }
  }


  this->_checkForNewFilesThread = std::thread(&DirectoryManager::checkForNewFiles, this);
  

  // std::this_thread::sleep_for(std::chrono::seconds(15));
  this->_checkForRecentlyModifiedFilesThread = std::thread(&DirectoryManager::checkForRecentlyModifiedFiles, this);

}


DirectoryManager::~DirectoryManager()
{
  _checkForNewFiles_isRunning = false;
  _checkForNewFilesThread.join();

  _checkForRecentlyModifiedFiles_isRunning = false;
  _checkForRecentlyModifiedFilesThread.join();

  for(auto it = files.begin(); it != files.end(); ++it) {
    delete *it;
  }
  files.clear();

  if(directory) {
    closedir(directory);
  }
}


std::string DirectoryManager::getPath()
{
  return this->path;
}


std::vector<NimbusFile*> DirectoryManager::getFiles()
{
  return this->files;
}


void DirectoryManager::checkForNewFiles()
{
  this->_checkForNewFiles_isRunning = true;
  while(_checkForNewFiles_isRunning)
  {
    // resets directory stream position
    rewinddir(directory);

    // instantiate a NimbusFile for each file in the folder
    std::cout << "Reading sync directory entries" << std::endl;
    while(struct dirent *entry = readdir(directory))
    {
      // check whether the entry is a file
      if(entry->d_type == DT_REG) {
        // checks if we already created a NimbusFile for
        // that entry
        // to-do: use map for better runtime complexity
        NimbusFile *found = nullptr;
        files_mtx.lock();
        for(auto it = files.begin(); it != files.end(); ++it) {
          std::string fullname = (*it)->getName() + "." + (*it)->getExtension();
          if( strcmp( fullname.c_str(), entry->d_name ) == 0 ) {
            found = *it;
            break;
          }
        }
        files_mtx.unlock();

        if(found == nullptr) {
          // we couldn't find a NimbusFile for this entry
          std::cout << "File found inside sync dir: " 
                    << entry->d_name
                    << std::endl;

          NimbusFile* file = new NimbusFile(this->path + "/" + entry->d_name);
          
          files_mtx.lock();
          this->files.push_back(file);
          files_mtx.unlock();

          // sends new unsynchronized file to other endpoint
          this->session->sendFile(file->getFilePath().c_str());
        }
      }
    }
    


    std::this_thread::sleep_for(std::chrono::seconds(10));
  }
}

void DirectoryManager::checkForRecentlyModifiedFiles()
{
  std::cout << "Checking for recently modified files" << std::endl;
  

  this->_checkForRecentlyModifiedFiles_isRunning = true;
  while(_checkForRecentlyModifiedFiles_isRunning)
  {
    files_mtx.lock();
    for(auto it = files.begin(); it != files.end(); ++it) {
      NimbusFile *file = *it;
      if(file->wasRecentlyModified()) {
        std::cout << "Sending modified file: "
                  << file->getFilePath()
                  << std::endl;

        // send file to other endpoint
        this->session->sendFile(file->getFilePath().c_str());
        file->resetRecentlyModified();
      }
    }
    files_mtx.unlock();
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}