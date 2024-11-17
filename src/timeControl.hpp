#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>

struct TimeControl
{
  std::chrono::milliseconds remainingTimeMs;
  std::chrono::steady_clock::time_point lastUpdateTimePoint;
  bool isRunning;

  TimeControl();
  std::string getTimeString() const;
};

class ChessTimer
{
private:
  Game &game;
  std::chrono::milliseconds incrementMs;
  std::atomic<bool> isRunning;
  std::thread timerThread;
  std::mutex mtx;
  std::condition_variable cv;

  void updateTimeControl(TimeControl &);

public:
  ChessTimer() = delete;
  ChessTimer(Game &);

  void startPlayerTimer(TimeControl &);
  void stopPlayerTimer(TimeControl &);
  void start();
  void stop();

  ~ChessTimer() { stop(); }
};