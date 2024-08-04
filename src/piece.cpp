#include "game.h"
#include "logger.h"
#include "piece.h"
#include "utils.h"

#include <array>
#include <iostream>
#include <utility>
#include <vector>

Piece::Piece(Game &g) : game(g) {}

std::vector<int> Piece::linear_square_indexes(int index, const std::vector<std::pair<int, int>> &offsets)
{
  std::vector<int> res = {};

  char piece = game.piece_placement[index];
  char color = piece_color(piece);
  auto [file, rank] = index_to_file_rank(index);
  int target_index, target_file, target_rank;

  for (auto &offset : offsets)
  {
    target_file = file;
    target_rank = rank;
    while (true)
    {
      target_file += offset.first;
      target_rank += offset.second;

      if (target_file < 1 || 8 < target_file || target_rank < 1 || 8 < target_rank)
        break;

      target_index = file_rank_to_index({target_file, target_rank});

      char target_piece = game.piece_placement[target_index];
      if (is_chess_piece(target_piece))
      {
        if (piece_color(game.piece_placement[target_index]) != color)
          res.push_back(target_index);
        break;
      }

      res.push_back(target_index);
    }
  }

  return res;
}

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

  auto [file, rank] = index_to_file_rank(index);
  int target_index, target_file, target_rank;

  for (auto &offset : offsets)
  {
    target_file = file + offset.first;
    target_rank = rank + offset.second;

    if (1 <= target_file && target_file <= 8 && 1 <= target_rank && target_rank <= 8)
    {
      target_index = file_rank_to_index({target_file, target_rank});
      res.push_back(target_index);
    }
  }

  return res;
};

Bishop::Bishop(Game &g) : Piece(g) {}

std::vector<int> Bishop::legal_square_indexes(int index)
{
  const std::vector<std::pair<int, int>> offsets = {{{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};

  return linear_square_indexes(index, offsets);
}

Rook::Rook(Game &g) : Piece(g) {}

std::vector<int> Rook::legal_square_indexes(int index)
{
  const std::vector<std::pair<int, int>> offsets = {{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};

  return linear_square_indexes(index, offsets);
}

Queen::Queen(Game &g) : Piece(g) {}

std::vector<int> Queen::legal_square_indexes(int index)
{
  const std::vector<std::pair<int, int>> offsets = {{{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};

  return linear_square_indexes(index, offsets);
}