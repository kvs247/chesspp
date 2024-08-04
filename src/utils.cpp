#include "logger.h"
#include "utils.h"

#include <array>
#include <cctype>
#include <set>
#include <stdexcept>
#include <string>

bool is_chess_piece(const char c)
{
  static const std::set<char> chess_pieces{'p', 'r', 'n', 'b', 'k', 'q', 'P', 'R', 'N', 'B', 'K', 'Q'};
  return chess_pieces.find(c) != chess_pieces.end();
}

char piece_color(const char c)
{
  if (!is_chess_piece(c))
    throw std::invalid_argument("Argument is not a chess piece.");
  return ((std::tolower(c) == c) ? 'b' : 'w');
}

int algebraic_to_index(const std::string &algebraic_square)
{
  if (algebraic_square.size() != 2)
  {
    throw std::invalid_argument("Argument is not 2 characters long.");
  }

  char file_c = std::tolower(algebraic_square[0]);
  char rank_c = algebraic_square[1];

  if (file_c < 'a' || file_c > 'h' || rank_c < '1' || rank_c > '8')
  {
    throw std::invalid_argument("Argument is not a valid chess square.");
  }

  int file_i = file_c - 'a';
  int rank_i = rank_c - '0';

  return 8 * (8 - rank_i) + file_i;
}

std::string index_to_algebraic(int index)
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
    if (res_i == 64)
    {
      throw std::invalid_argument("Array elements exceeds 64.");
      break;
    }

    if (c == '/')
      continue;

    if (isdigit(c))
    {
      for (int i = 0; i != (c - '0'); ++i)
        res[res_i++] = '\0';
    }
    else
    {
      if (!is_chess_piece(c))
      {
        throw std::invalid_argument("Encountered invalid chess piece.");
      }

      res[res_i++] = c;
    }
  }

  if (res_i != 64)
  {
    throw std::invalid_argument("Incorrect number of array elements.");
  }

  return res;
}

std::string piece_placement_array_to_string(const std::array<char, 64> &a)
{
  std::string res;
  unsigned gap = 0;

  auto handle_gap = [&]()
  {
    if (gap > 0)
    {
      res += std::to_string(gap);
      gap = 0;
    }
  };

  char c;
  for (int i = 0; i != 64; i++)
  {
    if (i % 8 == 0 && i != 0)
    {
      handle_gap();
      res += '/';
    }

    c = a[i];
    if (c)
    {
      if (!is_chess_piece(c))
      {
        throw std::invalid_argument("Encountered invalid chess piece.");
      }
      handle_gap();
      res += c;
    }
    else
      ++gap;
  }
  handle_gap();

  return res;
}

FileRank index_to_file_rank(int index)
{
  if (index < 0 || 63 < index)
    throw std::out_of_range("Index is out of valid range [0-63].");

  int file = index % 8 + 1;
  int rank = 8 - index / 8;

  return {file, rank};
};

int file_rank_to_index(FileRank square)
{
  auto [file, rank] = square;
  if (file < 1 || 8 < file || rank < 1 || 8 < rank)
  {
    throw std::out_of_range("File and/or rank out of valid range [1-8].");
  }

  int res = --file + 8 * (8 - rank);
  return res;
};