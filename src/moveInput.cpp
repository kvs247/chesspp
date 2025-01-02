#include <algorithm>
#include <atomic>
#include <cctype>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <cstdlib>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <termios.h>
#include <thread>
#include <tuple>
#include <unistd.h>
#include <utility>

#include "config.hpp"
#include "game.hpp"
#include "logger.hpp"
#include "moveInput.hpp"
#include "timeControl.hpp"
#include "utils.hpp"

MoveInput::MoveInput(Game &g) : game(g) {}

void MoveInput::enableRawMode()
{
  tcgetattr(STDIN_FILENO, &originalTermios);
  struct termios rawTermios = originalTermios;

  rawTermios.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  rawTermios.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
  rawTermios.c_cflag |= (CS8);
  rawTermios.c_cc[VMIN] = 0;
  rawTermios.c_cc[VTIME] = 1;

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &rawTermios);

  // hide cursor
  write(STDOUT_FILENO, "\033[?25l", 6);
};

void MoveInput::disableRawMode()
{
  // show cursor
  write(STDOUT_FILENO, "\033[?25h", 6);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalTermios);
};

std::optional<std::string> MoveInput::collectUserInput(const std::string prompt, const size_t inputLength)
{
  std::string input;
  char c;

  while (!cancelInput)
  {
    if (read(STDIN_FILENO, &c, 1) == 1)
    {
      if (game.modalState == Game::ModalState::HELP)
      {
        if (c == 'x')
        {
          game.modalState = Game::ModalState::NONE;
        }
        else if (c == 'd')
        {
          game.userInput = "";
          game.message = "draw by agreement";
          game.modalState = Game::ModalState::NONE;
          game.isGameOver = true;
          cancelInput = true;
        }
        else if (c == 'r')
        {
          game.userInput = "";
          const std::string winningPlayer = game.state.activeColor == PieceColor::White ? "Black" : "White";
          game.message = winningPlayer + " won by resignation";
          game.modalState = Game::ModalState::NONE;
          game.isGameOver = true;
          cancelInput = true;
        }
      }
      else if ((c == '\r' || c == '\n') && input.length() == inputLength)
      {
        return input;
      }
      else if (c == 127) // backspace
      {
        if (!input.empty())
        {
          input.pop_back();
        }
      }
      else if (c == ' ')
      {
      }
      else if (c == 'j')
      {
        game.modalState = Game::ModalState::HELP;
      }
      else if (c == 'x')
      {
        MoveInput::disableRawMode();
        std::exit(0);
      }
      else if (isalnum(c) && input.length() < inputLength)
      {
        input += c;
      }
    }
    if (!game.isGameOver)
    {
      game.userInput = prompt + input;
    }
  }
  // return input;
  return std::nullopt;
};

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

        if (config.blackIsCpu && game.state.activeColor == PieceColor::Black)
        {
          std::tie(fromIdx, toIdx) = game.generateCpuMove(PieceColor::Black);
        }
        else if (config.whiteIsCpu && game.state.activeColor == PieceColor::White)
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

          const std::string activePlayer = game.state.activeColor == PieceColor::White ? "White's" : "Black's";
          try
          {
            const std::string fromPrompt = "Enter " + activePlayer + " From Square: ";
            const auto userFromSquare = collectUserInput(fromPrompt, 2);
            if (userFromSquare.has_value())
            {
              fromIdx = algebraicToIndex(userFromSquare.value());
            }
            else
            {
              return;
            }

            const std::string toPrompt = "Enter " + activePlayer + " To Square: ";
            const auto userToSquare = collectUserInput(toPrompt, 2);
            if (userToSquare.has_value())
            {
              toIdx = algebraicToIndex(userToSquare.value());
            }
            else
            {
              return;
            }

            game.userInput = "";
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
            if (shared.inputReceived || shared.outOfTime || game.isGameOver)
            {
              cv.notify_one();
              break;
            }

            if (timeControl.isEnabled && timeControl.isOutOfTime())
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
    cv.wait(lock, [this]() { return shared.inputReceived || shared.outOfTime || game.isGameOver; });

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

struct termios MoveInput::originalTermios;
