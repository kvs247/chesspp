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

const bool DEBUG = false;

// TimeControl

TimeControl::TimeControl() : remainingTimeMs(config.timeControl * 60 * 1000), isRunning(false) {}

struct TimeData TimeControl::getTimeData() const
{
  const auto totalSeconds = std::chrono::duration_cast<std::chrono::seconds>(remainingTimeMs).count();
  const int minutes = totalSeconds / 60;
  const int seconds = totalSeconds % 60;
  const int milliseconds = remainingTimeMs.count() % 1000;

  return {minutes, seconds, milliseconds};
};

std::string TimeControl::getFormattedTimeString() const
{
  const auto timeData = getTimeData();
  std::stringstream ss;

  ss << timeData.minutes << ":";
  ss << std::setfill('0') << std::setw(2) << timeData.seconds;

  if (60 * timeData.minutes + timeData.seconds < 20)
  {
    ss << "." << timeData.milliseconds / 100;
  }

  return ss.str();
};

std::string TimeControl::getAbsoluteTimeString() const
{
  const auto timeData = getTimeData();
  std::stringstream ss;
  ss << std::setfill('0') << std::setw(2) << timeData.minutes << ":" << std::setfill('0') << std::setw(2)
     << timeData.seconds << "." << std::setfill('0') << std::setw(3) << timeData.milliseconds;
  return ss.str();
};

bool TimeControl::isOutOfTime() const { return remainingTimeMs == std::chrono::milliseconds(0); };

// ChessTimer

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
          if (DEBUG)
          {
            logger.log("black: ", game.blackTime.getAbsoluteTimeString(),
                       " white: ", game.whiteTime.getAbsoluteTimeString());
          }

          {
            std::lock_guard<std::mutex> lock(mtx); // ensure player switch cannot occur while updating TimeControl's

            updateTimeControl(game.whiteTime);
            updateTimeControl(game.blackTime);
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
