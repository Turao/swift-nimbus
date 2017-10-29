#include "NimbusFile.h"

#include <iostream>
#include <string.h>
#include <thread>
#include <sys/stat.h> // to check modification data
#include <time.h> // to convert from time_t to calendar time (struct tm)
#include <errno.h>


NimbusFile::NimbusFile(std::string filepath) :
filepath (filepath)
{
  std::cout << "Initializing Nimbus file object" << std::endl;


  //*********************************************************
  struct stat file_stat;
  if(stat(filepath.c_str(), &file_stat) == -1) {
    std::cout << "Nimbus file initialization failure: " 
              << "[" << filepath << "]"
              << strerror(errno)
              << std::endl;
    exit(1);
  }


  std::cout << "File path: " << filepath << std::endl;
  std::cout << "File name: " << "" << std::endl;
  std::cout << "File extension: " << "" << std::endl;
  std::cout << "File size: " << file_stat.st_size << " bytes" << std::endl;
  

   
  std::cout << "Last modified at: " 
            // converts to calendar time type with gmtime_s
            << asctime(localtime(&file_stat.st_mtime))
            << std::endl;
  //*********************************************************

  std::size_t startOfFileName = filepath.find_last_of("/\\");
  std::size_t startOfFileExt =  filepath.find_last_of(".\\");
  std::string fileName = filepath.substr(startOfFileName + 1, startOfFileExt - 1);
  std::string fileExt = filepath.substr(startOfFileExt + 1);
  
  this->setFilePath(filepath);
  this->setName(fileName);
  this->setExtension(fileExt);
  this->setLastModified(asctime(localtime(&file_stat.st_mtime)));
  this->setSize((int) file_stat.st_size);
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
  struct tm tm;
  time_t lastModified;

  std::cout << "Initializing file observer thread" << std::endl;
  this->_observer_isRunning = true;

  while(_observer_isRunning)
  {
    strptime(this->getLastModified().c_str(), "%a %b %d %T %Y", &tm);
    lastModified = mktime(&tm);

    if(stat(this->getFilePath().c_str(), &file_stat) != -1) {
      if (difftime(file_stat.st_mtime, lastModified) > 0) {
        // file has changed, do something
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


std::string NimbusFile::getLastModified()
{
  return this->lastModified;
}


void NimbusFile::setLastModified(std::string lastModified)
{
  this->lastModified = lastModified;
}


int NimbusFile::getSize()
{
  return this->size;
}


void NimbusFile::setSize(int size)
{
  this->size = size;
}