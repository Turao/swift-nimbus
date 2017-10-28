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
            << asctime(localtime(&file_stat.st_mtim.tv_sec))
            << std::endl;
  //*********************************************************
  


  // to-do: get file metadata
  // and store into private variables
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
  std::cout << "Initializing file observer thread" << std::endl;
  this->_observer_isRunning = true;
  while(_observer_isRunning)
  {

    // do something
    // check wether file status was modified
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}



std::string NimbusFile::getFilePath()
{
  return this->filepath;
}


std::string NimbusFile::getName()
{
  return this->name;
}


std::string NimbusFile::getExtension()
{
  return this->extension;
}


std::string NimbusFile::getLastModified()
{
  return this->lastModified;
}


int NimbusFile::getSize()
{
  return this->size;
}