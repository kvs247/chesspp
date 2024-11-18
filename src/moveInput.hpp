#pragma once

#include <optional>
#include <utility>
#include <condition_variable>
#include <mutex>
#include <atomic>

#include "game.hpp"
#include "types.hpp"

class Game;

class MoveInput
{
private:
  Game &game;
  std::condition_variable cv;
  std::mutex mtx;
  std::atomic<bool> cancelInput{false};
  struct {
    bool inputReceived = false;
    bool outOfTime = false;
    BoardIndex fromIndex;
    BoardIndex toIndex;
  } shared;

public:
  MoveInput() = delete;
  MoveInput(Game &);

  std::optional<std::pair<BoardIndex, BoardIndex>> handleGetInput();
};
