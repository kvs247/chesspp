#include <iostream>
#include <stdexcept>

#include "constants.hpp"
#include "draw.hpp"
#include "game.hpp"
#include "logger.hpp"
#include "utils.hpp"

#define LOG_FEN false

int main()
{
  Game game;

  while (!game.isGameOver)
  {
    if (LOG_FEN)
      logger.log(game.getFenStr());

    clearScreen();
    draw(game);

    try
    {
      if (game.move())
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
      logger.log("ERROR invalid argument encountered: " +
                 std::string(e.what()));
      continue;
    }
  }

  logger.log("yo");
  for (auto ftIndex : game.moveList)
  {
    logger.log(ftIndex.first, " ", ftIndex.second);
  }

  return 0;
}