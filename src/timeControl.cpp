#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

#include "config.hpp"
#include "game.hpp"
#include "timeControl.hpp"

TimeControl::TimeControl() : remainingTimeMs(config.timeControl * 60 * 1000), isRunning(false) {}

std::string TimeControl::getTimeString() const
{
  const auto totalSeconds = std::chrono::duration_cast<std::chrono::seconds>(remainingTimeMs).count();
  const int minutes = totalSeconds / 60;
  const int seconds = totalSeconds % 60;
  const int milliseconds = remainingTimeMs.count() % 1000;

  std::stringstream ss;
  ss << std::setfill('0') << std::setw(2) << minutes << ":" << std::setfill('0') << std::setw(2) << seconds << "."
     << std::setfill('0') << std::setw(3) << milliseconds;
  return ss.str();
};

void ChessTimer::updateTimeControl(TimeControl &timeControl)
{
  if (timeControl.isRunning)
  {
    const auto now = std::chrono::steady_clock::now();
    const auto elapsedTime = now - timeControl.lastUpdateTimePoint;
    const auto elapsedTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime);

    timeControl.remainingTimeMs -= elapsedTimeMs;
    timeControl.lastUpdateTimePoint = now;

    if (timeControl.remainingTimeMs <= std::chrono::milliseconds(0))
    {
      timeControl.remainingTimeMs = std::chrono::milliseconds(0);
      timeControl.isRunning = false;
    }
  }
}

ChessTimer::ChessTimer(Game &g) : game(g), incrementMs(config.incrementTime * 1000), isRunning(true) {}

void ChessTimer::startPlayerTimer(TimeControl &timeControl)
{
  std::lock_guard<std::mutex> lock(mtx);
  timeControl.lastUpdateTimePoint = std::chrono::steady_clock::now();
  timeControl.isRunning = true;
}

void ChessTimer::stopPlayerTimer(TimeControl &timeControl)
{
  std::lock_guard<std::mutex> lock(mtx);
  updateTimeControl(timeControl);
  timeControl.isRunning = false;
  timeControl.remainingTimeMs += incrementMs;
}

void ChessTimer::start()
{
  timerThread = std::thread(
      [this]()
      {
        while (isRunning)
        {
          logger.log("black: ", game.blackTime.getTimeString(), " white: ", game.whiteTime.getTimeString());
          {
            std::lock_guard<std::mutex> lock(mtx); // ensure player switch cannot occur while updating TimeControl's

            updateTimeControl(game.whiteTime);
            updateTimeControl(game.blackTime);

            if (game.whiteTime.isRunning && game.whiteTime.remainingTimeMs <= std::chrono::milliseconds(0))
            {
              game.isGameOver = true;
              game.message = "Black wins on time";
              break;
            }
            if (game.blackTime.isRunning && game.blackTime.remainingTimeMs <= std::chrono::milliseconds(0))
            {
              game.isGameOver = true;
              game.message = "White wins on time";
              break;
            }

            // draw(game);
          }

          std::unique_lock<std::mutex> lock(mtx);
          cv.wait_for(lock, std::chrono::milliseconds(100), [this]() { return !isRunning; });
        }
      });
}

void ChessTimer::stop()
{
  isRunning = false;
  cv.notify_one();
  if (timerThread.joinable())
  {
    timerThread.join();
  }
}
