#pragma once

#include <string>

#include <thread>
#include <atomic>

class NimbusFile
{
public:
  NimbusFile(std::string filepath);
  ~NimbusFile();

  std::string getFilePath();
  std::string getName();
  std::string getExtension();
  std::string getLastModified();
  int getSize();

  void setFilePath(std::string filepath);
  void setName(std::string name);
  void setExtension(std::string extension);
  void setLastModified(std::string lastModified);
  void setSize(int size);

  void startObserver();
  void stopObserver();

protected:

private: 
  std::string filepath;
  std::string name;
  std::string extension;
  std::string lastModified;
  int size;

  std::atomic<bool> _observer_isRunning;
  std::thread observerThread;
  void observer();
};
