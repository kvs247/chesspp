#pragma once

#include <algorithm>
#include <cctype>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
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

enum class ConfigKey
{
  WHITE_IS_CPU,
  BLACK_IS_CPU,
  DISABLE_TURN_ORDER,
  LOG_FEN,
  CPU_MOVE_DELAY_MS,
  SHOW_MOVE_LIST,
  STARTING_FEN,
  UNKNOWN,
};

bool parseBoolean(const std::string);

int parseInt(const std::string);

inline ConfigKey stringToConfigKey(const std::string key)
{
  static const std::map<std::string, ConfigKey> configKeyMap = {
      {"WHITE_IS_CPU", ConfigKey::WHITE_IS_CPU},
      {"BLACK_IS_CPU", ConfigKey::BLACK_IS_CPU},
      {"DISABLE_TURN_ORDER", ConfigKey::DISABLE_TURN_ORDER},
      {"LOG_FEN", ConfigKey::LOG_FEN},
      {"CPU_MOVE_DELAY_MS", ConfigKey::CPU_MOVE_DELAY_MS},
      {"SHOW_MOVE_LIST", ConfigKey::SHOW_MOVE_LIST},
      {"STARTING_FEN", ConfigKey::STARTING_FEN},
  };
  const auto it = configKeyMap.find(key);
  return it != configKeyMap.cend() ? it->second : ConfigKey::UNKNOWN;
}

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
    const auto splitIndex = line.find('=');
    if (splitIndex == std::string::npos)
    {
      throw std::invalid_argument("no '=' found in config line");
    }

    const std::string key = line.substr(0, splitIndex);
    const std::string value = line.substr(splitIndex + 1);

    switch (stringToConfigKey(key))
    {
    case ConfigKey::WHITE_IS_CPU:
      config.whiteIsCpu = parseBoolean(value);
      break;
    case ConfigKey::BLACK_IS_CPU:
      config.blackIsCpu = parseBoolean(value);
      break;
    case ConfigKey::DISABLE_TURN_ORDER:
      config.disableTurnOrder = parseBoolean(value);
      break;
    case ConfigKey::LOG_FEN:
      config.logFen = parseBoolean(value);
      break;
    case ConfigKey::CPU_MOVE_DELAY_MS:
      config.cpuMoveDelayMs = parseInt(value);
      break;
    case ConfigKey::SHOW_MOVE_LIST:
      config.showMoveList = parseBoolean(value);
      break;
    case ConfigKey::STARTING_FEN:
      config.startingFen = value.empty() ? startingFenString : value;
      break;
    default:
      throw std::invalid_argument("unknown config key encountered");
    }
  }

  return config;
}

inline bool parseBoolean(const std::string value)
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

inline int parseInt(const std::string value)
{
  if (value.empty())
  {
    throw std::invalid_argument("empty string");
  }

  if (std::find_if(value.cbegin(), value.cend(), [](char c) { return !std::isdigit(c); }) != value.cend())
  {
    throw std::invalid_argument("value is not a number");
  }

  return std::stoi(value);
}

inline Config config = loadConfig();
