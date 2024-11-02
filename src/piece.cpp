#include "piece.hpp"

#include <algorithm>
#include <array>
#include <iostream>
#include <iterator>
#include <utility>
#include <vector>

#include "game.hpp"
#include "logger.hpp"
#include "types.hpp"
#include "utils.hpp"

Piece::Piece(Game &g) : game(g) {}

std::vector<BoardIndex> Piece::linearSquareIndexes(
    const BoardIndex index, const PieceColor color,
    const std::vector<std::pair<int, int>> &offsets,
    const PiecePlacement &piecePlacement)
{
  std::vector<BoardIndex> res = {};

  auto [file, rank] = indexToFileRank(index);
  int targetFile,
      targetRank; // not FileRankIndex since we are testing if on board
  BoardIndex targetIndex;

  for (auto &offset : offsets)
  {
    targetFile = file;
    targetRank = rank;

    while (true)
    {
      targetFile += offset.first;
      targetRank += offset.second;

      if (targetFile < 1 || 8 < targetFile || targetRank < 1 || 8 < targetRank)
        break;

      targetIndex = fileRankToIndex({targetFile, targetRank});

      auto targetPiece = piecePlacement[targetIndex];
      if (targetPiece != ChessPiece::Empty)
      {
        if (pieceColor(targetPiece) != color)
        {
          res.push_back(targetIndex);
        }
        break;
      }

      res.push_back(targetIndex);
    }
  }

  return res;
}

std::vector<BoardIndex> Piece::squareIndexes(
    const BoardIndex index,
    const std::vector<std::pair<int, int>> &offsets,
    const PiecePlacement &piecePlacement,
    std::optional<PieceColor> currentPieceColor)
{
  std::vector<BoardIndex> res = {};

  auto [file, rank] = indexToFileRank(index);
  BoardIndex targetIndex;
  int targetFile, targetRank; // not FileRankIndex since we are testing if on board

  for (auto &offset : offsets)
  {
    targetFile = file + offset.first;
    targetRank = rank + offset.second;

    if (targetFile < 1 || 8 < targetFile || targetRank < 1 || 8 < targetRank)
    {
      continue;
    }

    targetIndex = fileRankToIndex({targetFile, targetRank});
    auto targetPiece = piecePlacement[targetIndex];
    if (currentPieceColor == std::nullopt)
    {
      const auto currentPiece = piecePlacement[index];
      currentPieceColor = pieceColor(currentPiece);
    }

    if (targetPiece != ChessPiece::Empty &&
        currentPieceColor == pieceColor(targetPiece))
    {
      continue;
    }

    res.push_back(targetIndex);
  }

  return res;
}

std::vector<BoardIndex> Piece::filterSelfCheckMoves(
    const PiecePlacement &piecePlacement, const BoardIndex index,
    const std::vector<BoardIndex> &indexes)
{
  auto isKingInCheckLambda = [piecePlacement, index](BoardIndex toIndex)
  {
    auto color = pieceColor(piecePlacement[index]);
    auto newPiecePlacement = piecePlacement;
    newPiecePlacement[toIndex] = newPiecePlacement[index];
    newPiecePlacement[index] = ChessPiece::Empty;
    return !Game::isKingInCheck(color, newPiecePlacement);
  };

  std::vector<BoardIndex> legalIndexes;
  std::copy_if(indexes.cbegin(), indexes.cend(),
               std::back_inserter(legalIndexes), isKingInCheckLambda);

  return legalIndexes;
};

Pawn::Pawn(Game &g) : Piece(g) {}

std::vector<BoardIndex> Pawn::legalSquareIndexes(const BoardIndex index) const
{
  std::vector<BoardIndex> potentialIndexes;

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
  if (game.piecePlacement[targetIndex] == ChessPiece::Empty)
  {
    potentialIndexes.push_back(targetIndex);
    // 2 ranks
    if ((color == PieceColor::White && rank == whiteStartRank) ||
        (color == PieceColor::Black && rank == blackStartRank))
    {
      targetIndex = fileRankToIndex({file, rank + 2 * sign});
      if (game.piecePlacement[targetIndex] == ChessPiece::Empty)
        potentialIndexes.push_back(targetIndex);
    }
  }

  // capture
  std::array<int, 2> offsets = {1, -1};
  for (auto &offset : offsets)
  {
    int targetFile =
        file + offset; // not FileRankIndex since we are testing if on board
    if (targetFile < 1 || 8 < targetFile)
      break;
    targetIndex = fileRankToIndex({file + offset, rank + sign});
    targetPiece = game.piecePlacement[targetIndex];
    if (targetPiece != ChessPiece::Empty && pieceColor(targetPiece) != color)
      potentialIndexes.push_back(targetIndex);
    // en passant
    if (game.enPassantIndex == targetIndex)
      potentialIndexes.push_back(targetIndex);
  }

  auto legalIndexes =
      filterSelfCheckMoves(game.piecePlacement, index, potentialIndexes);

  return legalIndexes;
}

Knight::Knight(Game &g) : Piece(g) {}

std::vector<BoardIndex> Knight::legalSquareIndexes(
    const BoardIndex index) const
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

  auto potentialIndexes = squareIndexes(index, offsets, game.piecePlacement);

  auto legalIndexes =
      filterSelfCheckMoves(game.piecePlacement, index, potentialIndexes);

  return legalIndexes;
};

Bishop::Bishop(Game &g) : Piece(g) {}

std::vector<BoardIndex> Bishop::legalSquareIndexes(
    const BoardIndex index) const
{
  const auto color = pieceColor(game.piecePlacement[index]);
  const std::vector<std::pair<int, int>> offsets = {
      {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

  auto potentialIndexes =
      linearSquareIndexes(index, color, offsets, game.piecePlacement);

  auto legalIndexes =
      filterSelfCheckMoves(game.piecePlacement, index, potentialIndexes);

  return legalIndexes;
}

Rook::Rook(Game &g) : Piece(g) {}

std::vector<BoardIndex> Rook::legalSquareIndexes(const BoardIndex index) const
{
  const auto color = pieceColor(game.piecePlacement[index]);
  const std::vector<std::pair<int, int>> offsets = {
      {1, 0}, {-1, 0}, {0, 1}, {0, -1}};

  auto potentialIndexes =
      linearSquareIndexes(index, color, offsets, game.piecePlacement);

  auto legalIndexes =
      filterSelfCheckMoves(game.piecePlacement, index, potentialIndexes);

  return legalIndexes;
}

Queen::Queen(Game &g) : Piece(g) {}

std::vector<BoardIndex> Queen::legalSquareIndexes(
    const BoardIndex index) const
{
  const auto color = pieceColor(game.piecePlacement[index]);
  const std::vector<std::pair<int, int>> offsets = {
      {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

  auto potentialIndexes =
      linearSquareIndexes(index, color, offsets, game.piecePlacement);

  auto legalIndexes =
      filterSelfCheckMoves(game.piecePlacement, index, potentialIndexes);

  return legalIndexes;
}

King::King(Game &g) : Piece(g) {}

std::vector<BoardIndex> King::legalSquareIndexes(const BoardIndex index) const
{
  const std::vector<std::pair<int, int>> offsets = {
      {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

  auto potentialIndexes = squareIndexes(index, offsets, game.piecePlacement);

  if (game.castlingAvailability.whiteShort &&
      game.piecePlacement[61] == ChessPiece::Empty &&
      game.piecePlacement[62] == ChessPiece::Empty &&
      !Game::isSquareUnderAttack(61, PieceColor::White, game.piecePlacement) &&
      !Game::isSquareUnderAttack(62, PieceColor::White, game.piecePlacement))
  {
    potentialIndexes.push_back(62);
  }
  if (game.castlingAvailability.whiteLong &&
      game.piecePlacement[59] == ChessPiece::Empty &&
      game.piecePlacement[58] == ChessPiece::Empty &&
      game.piecePlacement[57] == ChessPiece::Empty &&
      !Game::isSquareUnderAttack(59, PieceColor::White, game.piecePlacement) &&
      !Game::isSquareUnderAttack(58, PieceColor::White, game.piecePlacement))
  {
    potentialIndexes.push_back(58);
  }
  if (game.castlingAvailability.blackShort &&
      game.piecePlacement[5] == ChessPiece::Empty &&
      game.piecePlacement[6] == ChessPiece::Empty &&
      !Game::isSquareUnderAttack(5, PieceColor::Black, game.piecePlacement) &&
      !Game::isSquareUnderAttack(6, PieceColor::Black, game.piecePlacement))
  {
    potentialIndexes.push_back(6);
  }
  if (game.castlingAvailability.blackLong &&
      game.piecePlacement[3] == ChessPiece::Empty &&
      game.piecePlacement[2] == ChessPiece::Empty &&
      game.piecePlacement[1] == ChessPiece::Empty &&
      !Game::isSquareUnderAttack(3, PieceColor::Black, game.piecePlacement) &&
      !Game::isSquareUnderAttack(2, PieceColor::Black, game.piecePlacement))
  {
    potentialIndexes.push_back(2);
  }

  auto legalIndexes =
    filterSelfCheckMoves(game.piecePlacement, index, potentialIndexes);

  return legalIndexes;
}
