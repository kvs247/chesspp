#include "game.hpp"
#include "logger.hpp"
#include "piece.hpp"
#include "utils.hpp"

#include <array>
#include <iostream>
#include <utility>
#include <vector>

Piece::Piece(Game &g) : game(g) {}

std::vector<BoardIndex> Piece::linear_square_indexes(
    const BoardIndex index,
    const std::vector<std::pair<int, int>> &offsets,
    const PiecePlacement &piece_placement)
{
  std::vector<BoardIndex> res = {};

  ChessPiece piece = piece_placement[index];
  auto color = piece_color(piece);
  auto [file, rank] = index_to_file_rank(index);
  int target_file, target_rank; // not FileRankIndex since we are testing if on board
  BoardIndex target_index;

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

      ChessPiece target_piece = piece_placement[target_index];
      if (target_piece != ChessPiece::Empty)
      {
        if (piece_color(piece_placement[target_index]) != color)
          res.push_back(target_index);
        break;
      }

      res.push_back(target_index);
    }
  }

  return res;
}

std::vector<BoardIndex> Piece::square_indexes(
    const BoardIndex index, const std::vector<std::pair<int, int>> &offsets)
{
  std::vector<BoardIndex> res = {};

  auto [file, rank] = index_to_file_rank(index);
  BoardIndex target_index;
  int target_file, target_rank; // not FileRankIndex since we are testing if on board

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
}

Pawn::Pawn(Game &g) : Piece(g) {}

std::vector<BoardIndex> Pawn::legal_square_indexes(const BoardIndex index) const
{
  std::vector<BoardIndex> res;

  ChessPiece piece = game.piece_placement[index];
  ChessPiece target_piece = ChessPiece::Empty;
  auto color = piece_color(piece);
  auto [file, rank] = index_to_file_rank(index);
  BoardIndex target_index;

  int sign = color == PieceColor::White ? 1 : -1;
  int black_start_rank = 7;
  int white_start_rank = 2;

  // 1 rank
  target_index = file_rank_to_index({file, rank + sign});
  if (game.piece_placement[target_index] == ChessPiece::Empty)
  {
    res.push_back(target_index);
    // 2 ranks
    if ((color == PieceColor::White && rank == white_start_rank) || (color == PieceColor::Black && rank == black_start_rank))
    {
      target_index = file_rank_to_index({file, rank + 2 * sign});
      if (game.piece_placement[target_index] == ChessPiece::Empty)
        res.push_back(target_index);
    }
  }

  // capture
  std::array<int, 2> offsets = {1, -1};
  for (auto &offset : offsets)
  {
    int target_file = file + offset; // not FileRankIndex since we are testing if on board
    if (target_file < 1 || 8 < target_file)
      break;
    target_index = file_rank_to_index({file + offset, rank + sign});
    target_piece = game.piece_placement[target_index];
    if (target_piece != ChessPiece::Empty && piece_color(target_piece) != color)
      res.push_back(target_index);
    // en passant
    if (game.en_passant_index == target_index)
      res.push_back(target_index);
  }

  return res;
}

Knight::Knight(Game &g) : Piece(g) {}

std::vector<BoardIndex> Knight::legal_square_indexes(const BoardIndex index) const
{
  const std::vector<std::pair<int, int>> offsets = {
      {1, 2},
      {1, -2},
      {-1, 2},
      {-1, -2},
      {2, 1},
      {2, -1},
      {-2, 1},
      {-2, -1},
  };

  return square_indexes(index, offsets);
};

Bishop::Bishop(Game &g) : Piece(g) {}

std::vector<BoardIndex> Bishop::legal_square_indexes(const BoardIndex index) const
{
  const std::vector<std::pair<int, int>> offsets = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
  return linear_square_indexes(index, offsets, game.piece_placement);
}

Rook::Rook(Game &g) : Piece(g) {}

std::vector<BoardIndex> Rook::legal_square_indexes(const BoardIndex index) const
{
  const std::vector<std::pair<int, int>> offsets = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
  return linear_square_indexes(index, offsets, game.piece_placement);
}

Queen::Queen(Game &g) : Piece(g) {}

std::vector<BoardIndex> Queen::legal_square_indexes(const BoardIndex index) const
{
  const std::vector<std::pair<int, int>> offsets = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
  return linear_square_indexes(index, offsets, game.piece_placement);
}

King::King(Game &g) : Piece(g) {}

std::vector<BoardIndex> King::legal_square_indexes(const BoardIndex index) const
{
  const std::vector<std::pair<int, int>> offsets = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
  return square_indexes(index, offsets);
}