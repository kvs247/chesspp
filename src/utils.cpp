#include "utils.h"
#include <string>
#include <cctype>

int square_to_index(std::string square)
{
  int file = std::tolower(square[0]) - 'a';
  int rank = (square[1] - '0');
  return 8 * (8 - rank) + file;
}

std::string index_to_square(int index)
{
  int file_i = index % 8;
  int rank_i = index / 8;

  char file_c = file_i + 'a';
  char rank_c = (8 - rank_i) + '0';

  return std::string(1, file_c) + std::string(1, rank_c);
}