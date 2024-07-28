#include "utils.h"
#include <string>
#include <cctype>
#include <array>
#include <stdexcept>

int square_to_index(const std::string square)
{
  int file = std::tolower(square[0]) - 'a';
  int rank = (square[1] - '0');
  return 8 * (8 - rank) + file;
}

std::string index_to_square(int index)
{
  if (index < 0 || 63 < index)
  {
    throw std::out_of_range("Index is out of valid range [0-63].");
  }

  int file_i = index % 8;
  int rank_i = index / 8;

  char file_c = file_i + 'a';
  char rank_c = (8 - rank_i) + '0';

  return std::string(1, file_c) + std::string(1, rank_c);
}

std::array<char, 64> piece_placement_string_to_array(const std::string &s)
{
  std::array<char, 64> res;

  int res_i = 0;
  for (auto &c : s)
  {
    if (c == '/')
      continue;
    
    if (isdigit(c))
      for (int i = 0; i != c; ++i)
        res[++res_i] = '\0';
    
    else
      res[res_i] = c;
    ++res_i;
  }

  return res;
}