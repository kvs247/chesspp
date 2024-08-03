#include "game.h"
#include "logger.h"
#include "piece.h"
#include "utils.h"

#include <array>
#include <iostream>
#include <utility>
#include <vector>

Piece::Piece(Game &g) : game(g) {}

// Knight
Knight::Knight(Game &g) : Piece(g) {}

std::vector<int> Knight::legal_square_indexes(int index)
{
  std::vector<int> res = {};

  const std::array<std::pair<int, int>, 8> offsets = {{
      {1, 2},
      {1, -2},
      {-1, 2},
      {-1, -2},
      {2, 1},
      {2, -1},
      {-2, 1},
      {-2, -1},
  }};

  FileRank square = index_to_file_rank(index);
  auto file = square.file;
  auto rank = square.rank;

  int to_index, to_file, to_rank;
  for (auto &offset : offsets)
  {
    to_file = file + offset.first;
    to_rank = rank + offset.second;

    if (1 <= to_file && to_file <= 8 && 1 <= to_rank && to_rank <= 8)
    {
      to_index = file_rank_to_index({to_file, to_rank});
      res.push_back(to_index);
    }
  }

  return res;
};