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

  template <typename... Args>
  bool log(const Args &...args)
  {
    std::lock_guard<std::mutex> lock(mutex);

    if (log_file.is_open())
    {
      std::ostringstream oss;
      (oss << ... << args);

      log_file << oss.str() << std::endl;
      log_file.flush();
      return true;
    }
    else
    {
      std::cerr << "Failed to log message: log file is not open." << std::endl;
      return false;
    }
  }

  bool isOpen() const { return log_file.is_open(); }

  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

private:
  Logger()
  {
    log_file.open("log.txt", std::ios::out | std::ios::trunc);
    if (!log_file.is_open())
    {
      std::cerr << "Failed to open log file." << std::endl;
    }
  }
  ~Logger()
  {
    if (log_file.is_open())
    {
      log_file.close();
    }
  }

  std::ofstream log_file;
  std::mutex mutex;
};

#define logger Logger::getInstance()
