#pragma once

#include <algorithm>
#include <cctype>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

#include "constants.hpp"
#include "logger.hpp"
#include "types.hpp"

struct Config
{
  bool whiteIsCpu = false;
  bool blackIsCpu = false;
  int cpuMoveDelayMs = 1000;
  bool disableTurnOrder = false;
  bool logFen = false;
  bool showMoveList = true;
  std::string startingFen = startingFenString;
};

bool parseBoolean(std::string);

int parseInt(std::string);

inline Config loadConfig()
{
  Config config;

  std::ifstream file("config/config.txt");
  if (!file.is_open())
  {
    throw std::runtime_error("Failed to open config file");
  }

  std::string line;
  while (getline(file, line))
  {
    auto splitIndex = line.find('=');
    if (splitIndex == std::string::npos)
    {
      throw std::invalid_argument("no '=' found in config line");
    }

    std::string key = line.substr(0, splitIndex);
    std::string value = line.substr(splitIndex + 1);

    if (key == "WHITE_IS_CPU")
    {
      config.whiteIsCpu = parseBoolean(value);
    }
    if (key == "BLACK_IS_CPU")
    {
      config.blackIsCpu = parseBoolean(value);
    }
    if (key == "DISABLE_TURN_ORDER")
    {
      config.disableTurnOrder = parseBoolean(value);
    }
    if (key == "LOG_FEN")
    {
      config.logFen = parseBoolean(value);
    }
    if (key == "CPU_MOVE_DELAY_MS")
    {
      config.cpuMoveDelayMs = parseInt(value);
    }
    if (key == "SHOW_MOVE_LIST")
    {
      config.showMoveList = parseBoolean(value);
    }
    if (key == "STARTING_FEN")
    {
      config.startingFen = value.empty() ? startingFenString : value;
    }
  }

  return config;
}

inline bool parseBoolean(std::string value)
{
  if (value == "true")
  {
    return true;
  }
  if (value == "false")
  {
    return false;
  }

  throw std::invalid_argument("value is not 'true' or 'false'");
}

inline int parseInt(std::string value)
{
  if (value.empty())
  {
    throw std::invalid_argument("empty string");
  }

  if (std::find_if(value.cbegin(), value.cend(),
                   [](char c)
                   { return !std::isdigit(c); }) != value.cend())
  {
    throw std::invalid_argument("value is not a number");
  }

  return std::stoi(value);
}

inline Config config = loadConfig();
