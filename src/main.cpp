#include <iostream>
#include <stdexcept>

#include "constants.hpp"
#include "draw.hpp"
#include "game.hpp"
#include "logger.hpp"
#include "timeControl.hpp"
#include "utils.hpp"

int main()
{
  logger.log("");
  Game game(config.startingFen);

  TimeControl whiteTime(10, 0);
  TimeControl blackTime(10, 0);

  // create and start the timer thread
  ChessTimer timer(game, whiteTime, blackTime);
  timer.start();

  whiteTime.startTimer();

  draw(game);

  while (!game.isGameOver)
  {
    try
    {
      const bool isWhiteMove = game.isWhiteMove();

      game.processNextMove();

      if (isWhiteMove)
      {
        whiteTime.stopTimer();
        blackTime.startTimer();
      }
      else
      {
        blackTime.stopTimer();
        whiteTime.startTimer();
      }

      draw(game);
    }
    catch (std::invalid_argument &e)
    {
      game.message = "Invalid input, please try again.";
      logger.log("ERROR invalid argument encountered: " + std::string(e.what()));
      continue;
    }

    if (config.logFen)
    {
      logger.log(game.getFenStr());
    }
  }

  timer.stop();
  whiteTime.stopTimer();
  blackTime.stopTimer();
  draw(game);

  return 0;
}