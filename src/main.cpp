#include <iostream>
#include <stdexcept>

#include "draw.h"
#include "game.h"
#include "utils.h"
#include "logger.h"
#include "constants.h"

#define LOG_FEN false

int main()
{
  Game game;

  std::string from_square, to_square;
  std::string message = "";

  while (true)
  {
    if (LOG_FEN)
      logger.log(game.get_fen_str());

    clear_screen();
    draw(game.get_piece_placement(), message);

    try
    {
      if (game.move())
      {
        message = "";
      }
      else
      {
        message = "Illegal move.";
      }
    }
    catch (std::invalid_argument &e)
    {
      message = "Invalid input, please try again.";
      logger.log("ERROR invalid argument encountered: " + std::string(e.what()));
      continue;
    }
  }

  return 0;
}