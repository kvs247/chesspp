#include "logger.h"
#include <iostream>
#include <fstream>

Logger *Logger::instance = nullptr;
std::mutex Logger::instance_mutex;

Logger &logger = Logger::getInstance();

Logger &Logger::getInstance()
{
  std::lock_guard<std::mutex> lock(instance_mutex);
  if (!instance)
    instance = new Logger();
  return *instance;
}

Logger::Logger()
{

  std::ofstream clear_file("debug.log", std::ofstream::out | std::ofstream::trunc);
  if (clear_file.is_open())
  {
    clear_file << "";
    clear_file.close();
  }

  log_file.open("debug.log", std::ios::app);
}

bool Logger::log(const std::string &message)
{
  std::lock_guard<std::mutex> lock(mutex);
  if (log_file.is_open())
  {
    log_file << message << std::endl;
    log_file.flush();
    return true;
  }
  else
  {
    std::cerr << "Failed to log message: " << message << std::endl;
    return false;
  }
}

Logger::~Logger()
{
  if (log_file.is_open())
  {
    log_file.close();
    std::cout << "Log file closed in destructor.\n";
  }
}