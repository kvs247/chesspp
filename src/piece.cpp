#include "piece.hpp"

#include <array>
#include <iostream>
#include <utility>
#include <vector>

#include "game.hpp"
#include "logger.hpp"
#include "types.hpp"
#include "utils.hpp"

Piece::Piece(Game &g) : game(g) {}

std::vector<BoardIndex> Piece::linearSquareIndexes(
    const BoardIndex index, const std::vector<std::pair<int, int>> &offsets,
    const PiecePlacement &piecePlacement) {
  std::vector<BoardIndex> res = {};

  ChessPiece piece = piecePlacement[index];
  auto color = pieceColor(piece);
  auto [file, rank] = indexToFileRank(index);
  int targetFile,
      targetRank;  // not FileRankIndex since we are testing if on board
  BoardIndex targetIndex;

  for (auto &offset : offsets) {
    targetFile = file;
    targetRank = rank;
    while (true) {
      targetFile += offset.first;
      targetRank += offset.second;

      if (targetFile < 1 || 8 < targetFile || targetRank < 1 || 8 < targetRank)
        break;

      targetIndex = fileRankToIndex({targetFile, targetRank});

      ChessPiece targetPiece = piecePlacement[targetIndex];
      if (targetPiece != ChessPiece::Empty) {
        if (pieceColor(piecePlacement[targetIndex]) != color)
          res.push_back(targetIndex);
        break;
      }

      res.push_back(targetIndex);
    }
  }

  return res;
}

std::vector<BoardIndex> Piece::squareIndexes(
    const BoardIndex index, const std::vector<std::pair<int, int>> &offsets) {
  std::vector<BoardIndex> res = {};

  auto [file, rank] = indexToFileRank(index);
  BoardIndex targetIndex;
  int targetFile,
      targetRank;  // not FileRankIndex since we are testing if on board

  for (auto &offset : offsets) {
    targetFile = file + offset.first;
    targetRank = rank + offset.second;

    if (1 <= targetFile && targetFile <= 8 && 1 <= targetRank &&
        targetRank <= 8) {
      targetIndex = fileRankToIndex({targetFile, targetRank});
      res.push_back(targetIndex);
    }
  }

  return res;
}

Pawn::Pawn(Game &g) : Piece(g) {}

std::vector<BoardIndex> Pawn::legalSquareIndexes(const BoardIndex index) const {
  std::vector<BoardIndex> res;

  ChessPiece piece = game.piecePlacement[index];
  ChessPiece targetPiece = ChessPiece::Empty;
  auto color = pieceColor(piece);
  auto [file, rank] = indexToFileRank(index);
  BoardIndex targetIndex;

  int sign = color == PieceColor::White ? 1 : -1;
  int blackStartRank = 7;
  int whiteStartRank = 2;

  // 1 rank
  targetIndex = fileRankToIndex({file, rank + sign});
  if (game.piecePlacement[targetIndex] == ChessPiece::Empty) {
    res.push_back(targetIndex);
    // 2 ranks
    if ((color == PieceColor::White && rank == whiteStartRank) ||
        (color == PieceColor::Black && rank == blackStartRank)) {
      targetIndex = fileRankToIndex({file, rank + 2 * sign});
      if (game.piecePlacement[targetIndex] == ChessPiece::Empty)
        res.push_back(targetIndex);
    }
  }

  // capture
  std::array<int, 2> offsets = {1, -1};
  for (auto &offset : offsets) {
    int targetFile =
        file + offset;  // not FileRankIndex since we are testing if on board
    if (targetFile < 1 || 8 < targetFile) break;
    targetIndex = fileRankToIndex({file + offset, rank + sign});
    targetPiece = game.piecePlacement[targetIndex];
    if (targetPiece != ChessPiece::Empty && pieceColor(targetPiece) != color)
      res.push_back(targetIndex);
    // en passant
    if (game.enPassantIndex == targetIndex) res.push_back(targetIndex);
  }

  return res;
}

Knight::Knight(Game &g) : Piece(g) {}

std::vector<BoardIndex> Knight::legalSquareIndexes(
    const BoardIndex index) const {
  const std::vector<std::pair<int, int>> offsets = {
      {1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
  };

  return squareIndexes(index, offsets);
};

Bishop::Bishop(Game &g) : Piece(g) {}

std::vector<BoardIndex> Bishop::legalSquareIndexes(
    const BoardIndex index) const {
  const std::vector<std::pair<int, int>> offsets = {
      {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
  return linearSquareIndexes(index, offsets, game.piecePlacement);
}

Rook::Rook(Game &g) : Piece(g) {}

std::vector<BoardIndex> Rook::legalSquareIndexes(const BoardIndex index) const {
  const std::vector<std::pair<int, int>> offsets = {
      {1, 0}, {-1, 0}, {0, 1}, {0, -1}};
  return linearSquareIndexes(index, offsets, game.piecePlacement);
}

Queen::Queen(Game &g) : Piece(g) {}

std::vector<BoardIndex> Queen::legalSquareIndexes(
    const BoardIndex index) const {
  const std::vector<std::pair<int, int>> offsets = {
      {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
  return linearSquareIndexes(index, offsets, game.piecePlacement);
}

King::King(Game &g) : Piece(g) {}

std::vector<BoardIndex> King::legalSquareIndexes(const BoardIndex index) const {
  const std::vector<std::pair<int, int>> offsets = {
      {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
  return squareIndexes(index, offsets);
}