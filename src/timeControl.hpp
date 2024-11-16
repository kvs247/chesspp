#include "game.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <thread>

struct TimeControl
{
  std::chrono::seconds initialTime;
  std::chrono::seconds increment;
  std::chrono::steady_clock::time_point lastMoveTime;
  std::chrono::seconds remainingTime;
  std::atomic<bool> isRunning;

  TimeControl(int initalMinutes = 10, int incrementSeconds = 0)
      : initialTime(initalMinutes * 60), increment(incrementSeconds), remainingTime(initalMinutes * 60),
        isRunning(false)
  {
  }

  void startTimer()
  {
    lastMoveTime = std::chrono::steady_clock::now();
    isRunning = true;
  }

  void stopTimer()
  {
    if (isRunning)
    {
      auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - lastMoveTime);
      remainingTime -= elapsed;
      remainingTime += increment;
      isRunning = false;
    }
  }

  std::string getTimeString() const
  {
    int minutes = remainingTime.count() / 60;
    int seconds = remainingTime.count() % 60;

    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << minutes << ":" << std::setfill('0') << std::setw(2) << seconds;
    return ss.str();
  };

  bool hasTimeLeft() const { return remainingTime.count() > 0; };

  void updateDisplay()
  {
    if (isRunning)
    {
      auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - lastMoveTime);
      auto currentTime = remainingTime - elapsed;
      if (currentTime.count() <= 0)
      {
        remainingTime = std::chrono::seconds(0);
        isRunning = false;
      }
    }
  }
};

class ChessTimer
{
private:
  Game &game;
  TimeControl &whiteTime;
  TimeControl &blackTime;
  std::atomic<bool> running;
  std::thread timerThread;
  std::mutex mtx;
  std::condition_variable cv;

public:
  ChessTimer(Game &g, TimeControl &wt, TimeControl &bt) : game(g), whiteTime(wt), blackTime(bt), running(true) {}

  void start()
  {
    timerThread = std::thread(
        [this]()
        {
          while (running)
          {
            {
              std::lock_guard<std::mutex> lock(mtx);
              whiteTime.updateDisplay();
              blackTime.updateDisplay();
              logger.log("white: ", whiteTime.getTimeString(), " black: ", blackTime.getTimeString());

              if (whiteTime.isRunning && !whiteTime.hasTimeLeft())
              {
                game.isGameOver = true;
                game.message = "Black wins on time";
                break;
              }
              if (blackTime.isRunning && !blackTime.hasTimeLeft())
              {
                game.isGameOver = true;
                game.message = "White wins on time";
                break;
              }

              // draw(game);
            }

            // wait for 1 second or util the timer is stopped
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait_for(lock, std::chrono::seconds(1), [this]() { return !running; });

          }
        });
  }

  void stop()
  {
    running = false;
    cv.notify_one();
    if (timerThread.joinable())
    {
      timerThread.join();
    }
  }

  ~ChessTimer() { stop(); }
};