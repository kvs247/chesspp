#pragma once

#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

class Logger
{
public:
  static Logger &getInstance()
  {
    static Logger instance;
    return instance;
  }

  template <typename... Args> bool log(const Args &...args)
  {
    std::lock_guard<std::mutex> lock(mutex);

    if (logFile.is_open())
    {
      std::ostringstream oss;
      (oss << ... << args);

      logFile << oss.str() << std::endl;
      logFile.flush();
      return true;
    }
    else
    {
      std::cerr << "Failed to log message: log file is not open." << std::endl;
      return false;
    }
  }

  bool isOpen() const { return logFile.is_open(); }

  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

private:
  Logger()
  {
    logFile.open("out/log.txt", std::ios::out | std::ios::trunc);
    if (!logFile.is_open())
    {
      std::cerr << "Failed to open log file." << std::endl;
    }
  }
  ~Logger()
  {
    if (logFile.is_open())
    {
      logFile.close();
    }
  }

  std::ofstream logFile;
  std::mutex mutex;
};

#define logger Logger::getInstance()
