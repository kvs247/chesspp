#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <string>
#include <utility>

#include "game.hpp"
#include "types.hpp"

#include <termios.h>

class Game;

class MoveInput
{
private:
  Game &game;
  std::condition_variable cv;
  std::mutex mtx;
  std::atomic<bool> cancelInput{false};
  static struct termios originalTermios;
  struct
  {
    bool inputReceived = false;
    bool outOfTime = false;
    BoardIndex fromIndex;
    BoardIndex toIndex;
  } shared;

public:
  MoveInput() = delete;
  MoveInput(Game &);

  ~MoveInput() { MoveInput::disableRawMode(); }

  static void enableRawMode();
  static void disableRawMode();
  std::optional<std::string> collectUserInput(const std::string);
  std::optional<std::pair<BoardIndex, BoardIndex>> handleGetInput();
};
