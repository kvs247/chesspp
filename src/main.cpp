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
  logger.log("");
  Game game;

  while (!game.isGameOver)
  {
    // logger.log("game: ", sizeof(game), " piecePlacement: ", sizeof(game.getPiecePlacement()));
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

  return 0;
}