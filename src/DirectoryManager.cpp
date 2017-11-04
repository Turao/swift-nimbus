#include "DirectoryManager.h"

#include <iostream>
#include <string.h>


#include <dirent.h> // directory opening and entries reading
#include <sys/stat.h> // to use mkdir


#define DEBUG_PATH_PREFIX "/home/leonardo/sync_dir_" // to-do: remove
#define PATH_PREFIX "/home/sync_dir_"

DirectoryManager::DirectoryManager(std::string username)
{
  this->path = DEBUG_PATH_PREFIX + username;
  std::cout << "Directory Manager initialized at: " 
            << this->path 
            << std::endl;

  // check directory existence
  DIR *directory = opendir(this->path.c_str());
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
      exit(1);
    }
  }


  // instantiate a NimbusFile for each file in the folder
  std::cout << "Reading sync directory entries" << std::endl;
  while(struct dirent *entry = readdir(directory))
  {
    // check whether the entry is a file
    if(entry->d_type == DT_REG) {
      std::cout << "File found inside sync dir: " 
                << entry->d_name
                << std::endl;
      NimbusFile* file = new NimbusFile(this->path + "/" + entry->d_name);
      this->files.push_back(file);
    }
  }


  closedir(directory);
}


DirectoryManager::~DirectoryManager()
{
  for(auto it = files.begin(); it != files.end(); ++it) {
    delete *it;
  }
  files.clear();
}


std::vector<NimbusFile*> DirectoryManager::getFiles()
{
  return this->files;
}


void DirectoryManager::notify()
{

}