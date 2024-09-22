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