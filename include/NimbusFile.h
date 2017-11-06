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
  time_t getLastModified();
  int getSize();
  int wasRecentlyModified();

  void setFilePath(std::string filepath);
  void setName(std::string name);
  void setExtension(std::string extension);
  void setLastModified(time_t lastModified);
  void setSize(int size);
  void resetRecentlyModified();

  void startObserver();
  void stopObserver();

protected:

private:
  std::string filepath;
  std::string name;
  std::string extension;
  time_t lastModified;
  int size;

  bool recentlyModified;

  std::atomic<bool> _observer_isRunning;
  std::thread observerThread;
  void observer();
};
