#include "NimbusFile.h"

#include <iostream>
#include <string.h>
#include <thread>
#include <sys/stat.h> // to check modification data
#include <time.h> // to convert from time_t to calendar time (struct tm)
#include <errno.h>


NimbusFile::NimbusFile(std::string filepath) :
filepath(filepath),
recentlyModified(false)
{
  struct stat file_stat;

  std::cout << "Initializing Nimbus file object" << std::endl;

  if(stat(filepath.c_str(), &file_stat) == -1) {
    std::cout << "Nimbus file initialization failure: " 
              << "[" << filepath << "]"
              << strerror(errno)
              << std::endl;
    exit(1);
  }

  std::size_t startOfFileName = filepath.find_last_of("/\\");
  std::size_t startOfFileExt =  filepath.find_last_of(".\\");
  std::string fileName = filepath.substr(startOfFileName + 1, startOfFileExt - startOfFileName - 1);
  std::string fileExt = filepath.substr(startOfFileExt + 1);
  
  this->setFilePath(filepath);
  this->setName(fileName);
  this->setExtension(fileExt);
  this->setLastModified(file_stat.st_mtime);
  this->setStatusChange(file_stat.st_ctime);
  this->setLastAccess(file_stat.st_atime);
  this->setSize((int) file_stat.st_size);

  std::cout << "File path: " << this->getFilePath() << std::endl;
  std::cout << "File name: " << this->getName() << std::endl;
  std::cout << "File extension: " << this->getExtension() << std::endl;
  std::cout << "File size: " << this->getSize() << " bytes" << std::endl;
  std::cout << "Last modified: " 
            << asctime(localtime(&file_stat.st_mtime))
            << std::endl;
  this->startObserver();
}


NimbusFile::~NimbusFile()
{
  if(this->_observer_isRunning) {
    std::cout << "Stopping file observer" 
              << std::endl;
    stopObserver();
  }
}


void NimbusFile::startObserver()
{
  this->observerThread = std::thread(&NimbusFile::observer, this);
}


void NimbusFile::stopObserver()
{
  this->_observer_isRunning = false;
  this->observerThread.join();
}


void NimbusFile::observer()
{
  struct stat file_stat;

  std::cout << "Initializing file observer thread" << std::endl;
  this->_observer_isRunning = true;

  while(_observer_isRunning)
  {
    if(stat(this->getFilePath().c_str(), &file_stat) != -1) {
      if (difftime(file_stat.st_mtime, this->getLastModified()) > 0) {
        this->setLastModified(file_stat.st_mtime);
        this->setSize((int) file_stat.st_size);
        std::cout << "File " << this->getName() << "." << this->getExtension() << " has changed" << std::endl;
        // notify directory manager
        this->recentlyModified = true;
      }
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}



std::string NimbusFile::getFilePath()
{
  return this->filepath;
}


void NimbusFile::setFilePath(std::string filepath)
{
  this->filepath = filepath;
}


std::string NimbusFile::getName()
{
  return this->name;
}


void NimbusFile::setName(std::string name)
{
  this->name = name;
}


std::string NimbusFile::getExtension()
{
  return this->extension;
}


void NimbusFile::setExtension(std::string extension)
{
  this->extension = extension;
}


time_t NimbusFile::getLastModified()
{
  return this->lastModified;
}


void NimbusFile::setLastModified(time_t lastModified)
{
  this->lastModified = lastModified;
}


time_t NimbusFile::getStatusChange()
{
  return this->statusChange;
}


void NimbusFile::setStatusChange(time_t statusChange)
{
  this->statusChange = statusChange;
}


time_t NimbusFile::getLastAccess()
{
  return this->lastAccess;
}


void NimbusFile::setLastAccess(time_t lastAccess)
{
  this->lastAccess = lastAccess;
}


int NimbusFile::getSize()
{
  return this->size;
}


void NimbusFile::setSize(int size)
{
  this->size = size;
}


int NimbusFile::wasRecentlyModified()
{
  return this->recentlyModified;
}


void NimbusFile::resetRecentlyModified()
{
  this->recentlyModified = false;
}