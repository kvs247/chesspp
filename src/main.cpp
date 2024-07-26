#include <iostream>
#include <unistd.h>
#include "draw.h"

#include <string>
#include <cctype>
// #include <algorithm>

char pieces[64] = {
    'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',
    'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
    'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'};

int square_to_int(std::string square)
{
  int file = std::tolower(square[0]) - 'a';
  int rank = (square[1] - '0');
  return 8 * (8 - rank) + file;
}

int main()
{
  std::string from_square, to_square;
  while (true)
  {
    draw(pieces);

    std::cin >> from_square >> to_square;
    int from_index = square_to_int(from_square);
    int to_index = square_to_int(to_square);
    std::cout << from_index << " " << to_index << '\n';

    pieces[to_index] = pieces[from_index];
    pieces[from_index] = '\0';
  }

  return 0;
}