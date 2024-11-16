#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <thread>

#include "game.hpp"

struct TimeControl
{
  std::chrono::milliseconds remainingTimeMs;
  std::chrono::steady_clock::time_point lastUpdateTimePoint;
  bool isRunning;

  TimeControl(int initalMinutes = 10) : remainingTimeMs(initalMinutes * 60 * 1000), isRunning(false) {}

  std::string getTimeString() const
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
};

class ChessTimer
{
private:
  Game &game;
  TimeControl &whiteTimeControl;
  TimeControl &blackTimeControl;
  std::chrono::milliseconds bonusTimeMs;
  std::atomic<bool> isRunning;
  std::thread timerThread;
  std::mutex mtx;
  std::condition_variable cv;

  void updateTimeControl(TimeControl &timeControl)
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

public:
  ChessTimer(Game &g, TimeControl &wtc, TimeControl &btc, int btMs = 0)
      : game(g), whiteTimeControl(wtc), blackTimeControl(btc), bonusTimeMs(btMs), isRunning(true)
  {
  }

  void startPlayerTimer(TimeControl &timeControl)
  {
    std::lock_guard<std::mutex> lock(mtx);
    timeControl.lastUpdateTimePoint = std::chrono::steady_clock::now();
    timeControl.isRunning = true;
  }

  void stopPlayerTimer(TimeControl &timeControl)
  {
    std::lock_guard<std::mutex> lock(mtx);
    updateTimeControl(timeControl);
    timeControl.isRunning = false;
    timeControl.remainingTimeMs += bonusTimeMs;
  }

  void start()
  {
    timerThread = std::thread(
        [this]()
        {
          while (isRunning)
          {
            logger.log("black: ", blackTimeControl.getTimeString(), " white: ", whiteTimeControl.getTimeString());
            {
              std::lock_guard<std::mutex> lock(mtx); // ensure player switch cannot occur while updating TimeControl's

              updateTimeControl(whiteTimeControl);
              updateTimeControl(blackTimeControl);

              if (whiteTimeControl.isRunning && whiteTimeControl.remainingTimeMs <= std::chrono::milliseconds(0))
              {
                game.isGameOver = true;
                game.message = "Black wins on time";
                break;
              }
              if (blackTimeControl.isRunning && blackTimeControl.remainingTimeMs <= std::chrono::milliseconds(0))
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

  void stop()
  {
    isRunning = false;
    cv.notify_one();
    if (timerThread.joinable())
    {
      timerThread.join();
    }
  }

  ~ChessTimer() { stop(); }
};