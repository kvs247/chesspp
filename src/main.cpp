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

  TimeControl whiteTime(10);
  TimeControl blackTime(10);

  // create and start the timer thread
  ChessTimer timer(game, whiteTime, blackTime);
  timer.start();
  timer.startPlayerTimer(whiteTime);

  draw(game);

  while (!game.isGameOver)
  {
    try
    {
      const bool isWhiteMove = game.isWhiteMove();
      game.processNextMove();

      if (isWhiteMove)
      {
        timer.stopPlayerTimer(whiteTime);
        timer.startPlayerTimer(blackTime);
      }
      else
      {
        timer.stopPlayerTimer(blackTime);
        timer.startPlayerTimer(whiteTime);
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
  draw(game);

  return 0;
}