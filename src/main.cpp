#include <iostream>
#include "draw.h"
#include "game.h"
#include "utils.h"
#include "logger.h"
#include "constants.h"

int main()
{
  Logger &logger = Logger::getInstance();

  Game game;
  logger.log(game.get_fen_str() + "\n");

  std::string from_square, to_square;
  int from_index, to_index;
  while (true)
  {
    clear_screen();
    draw(game.get_piece_placement());

    std::cout << "from square: ";
    std::cin >> from_square;
    std::cout << "\n";

    std::cout << "to square: ";
    std::cin >> to_square;
    std::cout << "\n";

    from_index = square_to_index(from_square);
    to_index = square_to_index(to_square);

    game.move(from_index, to_index);
  } 


  return 0;
}