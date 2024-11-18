#include <atomic>
#include <condition_variable>
#include <exception>
#include <future>
#include <iostream>
#include <mutex>
#include <optional>
#include <thread>
#include <utility>

#include "config.hpp"
#include "game.hpp"
#include "moveInput.hpp"

MoveInput::MoveInput(Game &g) : game(g) {}

std::pair<BoardIndex, BoardIndex> getUserMove(std::istream &is, std::ostream &os, const std::atomic<bool> &cancelInput)
{
  BoardIndex fromIndex, toIndex;
  std::string fromAlgebraic, toAlgebraic;

  auto getInput = [&](const std::string &prompt) -> std::optional<std::string>
  {
    os << prompt;

    std::promise<std::string> inputPromise;
    auto inputFuture = inputPromise.get_future();

    auto ioThread = std::thread(
        [&]()
        {
          std::string temp;
          is >> temp;
          inputPromise.set_value(temp);
        });

    // wait for input or cancel
    while (!cancelInput)
    {
      auto status = inputFuture.wait_for(std::chrono::milliseconds(100));
      if (status == std::future_status::ready)
      {
        ioThread.join();
        return inputFuture.get();
      }
    }

    // this implies cancellation
    ioThread.detach();
    return std::nullopt;
  };

  auto fromInput = getInput("from square: ");
  if (!fromInput)
  {
    throw std::runtime_error("input cancelled");
  }
  fromIndex = algebraicToIndex(fromInput.value());

  auto toInput = getInput("to square:");
  if (!toInput)
  {
    throw std::runtime_error("input cancelled");
  }
  toIndex = algebraicToIndex(toInput.value());

  return {fromIndex, toIndex};
}

std::optional<std::pair<BoardIndex, BoardIndex>> MoveInput::handleGetInput()
{
  {
    std::lock_guard<std::mutex> lock(mtx);
    shared.inputReceived = false;
    shared.outOfTime = false;
    cancelInput = false;
  }

  auto inputThread = std::thread(
      [this]()
      {
        BoardIndex fromIdx, toIdx;

        if (config.blackIsCpu && game.activeColor == PieceColor::Black)
        {
          std::tie(fromIdx, toIdx) = game.generateCpuMove(PieceColor::Black);
        }
        else if (config.whiteIsCpu && game.activeColor == PieceColor::White)
        {
          std::tie(fromIdx, toIdx) = game.generateCpuMove(PieceColor::White);
        }
        else
        {
          {
            std::lock_guard<std::mutex> lock(mtx);
            if (shared.outOfTime)
            {
              return;
            }
          }
          try
          {
            std::tie(fromIdx, toIdx) = getUserMove(std::cin, std::cout, cancelInput);
          }
          catch (const std::runtime_error &e)
          {
            return; // input was cancelled
          }
        }

        std::lock_guard<std::mutex> lock(mtx);
        if (!shared.outOfTime)
        {
          shared.fromIndex = fromIdx;
          shared.toIndex = toIdx;
          shared.inputReceived = true;
          cv.notify_one();
        }
      });

  auto timerThread = std::thread(
      [this]()
      {
        const auto &timeControl = game.isWhiteMove() ? game.whiteTime : game.blackTime;
        while (true)
        {
          {
            std::lock_guard<std::mutex> lock(mtx);
            if (shared.inputReceived || shared.outOfTime)
            {
              break;
            }

            if (timeControl.isOutOfTime())
            {
              shared.outOfTime = true;
              cancelInput = true; // Set to true to cancel input
              cv.notify_one();
              break;
            }
          }

          int64_t sleepMs = 100;
          if ((game.isWhiteMove() && config.whiteIsCpu) || (!game.isWhiteMove() && config.blackIsCpu))
          {
            sleepMs = std::min(config.cpuMoveDelayMs, 100);
          }
          std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
        }
      });

  // wait for result
  std::pair<BoardIndex, BoardIndex> result;
  bool success = false;
  {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this]() { return shared.inputReceived || shared.outOfTime; });

    if (shared.inputReceived)
    {
      result = {shared.fromIndex, shared.toIndex};
      success = true;
    }
  }

  if (inputThread.joinable())
  {
    inputThread.join();
  }
  if (timerThread.joinable())
  {
    timerThread.join();
  }

  return success ? std::optional<std::pair<BoardIndex, BoardIndex>>(result) : std::nullopt;
};