#include <iostream>
#include <unistd.h>
#include "draw.h"
#include "chess.h"
#include "utils.h"

char pieces[64] = {
    'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',
    'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
    'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'};

int main()
{
  Chess chess;
  std::cout << chess.get_fen_str() << '\n';

  std::string from_square, to_square;
  while (true)
  {
    // clear_screen();
    draw(pieces);

    std::cin >> from_square >> to_square;
    int from_index = square_to_index(from_square);
    int to_index = square_to_index(to_square);
    std::cout << from_index << " " << to_index << '\n';

    pieces[to_index] = pieces[from_index];
    pieces[from_index] = '\0';
  }

  return 0;
}