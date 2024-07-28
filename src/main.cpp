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
    draw(test_piece_placement);

    std::cin >> from_square >> to_square;
    int from_index = square_to_index(from_square);
    int to_index = square_to_index(to_square);
    std::cout << from_index << " " << to_index << '\n';

    test_piece_placement[to_index] = test_piece_placement[from_index];
    test_piece_placement[from_index] = '\0';
  } 


  return 0;
}