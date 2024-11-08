#include <iostream>
#include <stdexcept>

#include "constants.hpp"
#include "draw.hpp"
#include "game.hpp"
#include "logger.hpp"
#include "utils.hpp"

int main()
{
  logger.log("");
  Game game(config.startingFen);

  while (!game.isGameOver)
  {
    if (config.logFen)
    {
      logger.log(game.getFenStr());
    }

    clearScreen();
    draw(game);

    try
    {

      if (game.processNextMove())
      {
        game.message = "";
      }
      else
      {
        game.message = "Illegal move.";
      }
    }
    catch (std::invalid_argument &e)
    {
      game.message = "Invalid input, please try again.";
      logger.log("ERROR invalid argument encountered: " + std::string(e.what()));
      continue;
    }
  }

  return 0;
}