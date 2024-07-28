#include "utils.h"
#include <string>
#include <cctype>
#include <array>
#include <stdexcept>

int square_to_index(const std::string &square)
{
  if (square.size() != 2)
  {
    throw std::invalid_argument("Argument is not 2 characters long.");
  }

  char file_c = std::tolower(square[0]);
  char rank_c = square[1];

  if (file_c < 'a' || file_c > 'h' || rank_c < '1' || rank_c > '8')
  {
    throw std::invalid_argument("Argument is not a valid chess square.");
  }

  int file_i = file_c - 'a';
  int rank_i = rank_c - '0';

  return 8 * (8 - rank_i) + file_i;
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

  size_t res_i = 0;
  for (auto &c : s)
  {
    if (c == '/')
      continue;

    if (isdigit(c))
    {
      for (int i = 0; i != (c - '0'); ++i)
        res[res_i++] = '\0';
    }
    else
      res[res_i++] = c;

    // add error handling
  }

  return res;
}
