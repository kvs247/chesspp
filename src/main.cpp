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
  draw(game);

  while (!game.isGameOver)
  {
    try
    {
      game.processNextMove();
      draw(game);
    }
    catch (std::invalid_argument &e)
    {
      game.message = "Invalid input, please try again.";
      logger.log("ERROR invalid argument encountered: " + std::string(e.what()));
      continue;
    }

    draw(game);

    if (config.logFen)
    {
      logger.log(game.getFenStr());
    }
  }
  return 0;
}