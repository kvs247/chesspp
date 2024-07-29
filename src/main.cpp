#include <iostream>
#include "draw.h"
#include "game.h"
#include "utils.h"
#include "logger.h"
#include "constants.h"

#include <array>
std::array<char, 64> test_piece_placement = starting_piece_placement;

int main()
{
  Logger &logger = Logger::getInstance();

  Game game;
  logger.log(game.get_fen_str() + "\n");

  std::string from_square, to_square;
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

    int from_index = square_to_index(from_square);
    int to_index = square_to_index(to_square);

    game.move(from_index, to_index);
  } 


  return 0;
}