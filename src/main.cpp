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
  int from_index, to_index;

  while (true)
  {
    if (LOG_FEN)
      logger.log(game.get_fen_str());

    clear_screen();
    draw(game.get_piece_placement(), message);

    try
    {
      game.read_move(from_index, to_index);
      message = "";
    }
    catch (std::invalid_argument &e)
    {
      message = "Invalid input, please try again.";
      logger.log("ERROR invalid argument encountered: " + std::string(e.what()));
      continue;
    }

    game.move(from_index, to_index);
  }

  return 0;
}