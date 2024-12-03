#include "piece.hpp"

#include <algorithm>
#include <array>
#include <iterator>
#include <optional>
#include <utility>
#include <vector>

#include "game.hpp"
#include "types.hpp"
#include "utils.hpp"

Piece::Piece(Game &g) : game(g) {}

std::vector<BoardIndex> Piece::linearSquareIndexes(
    const BoardIndex index,
    const PieceColor color,
    const std::vector<std::pair<int, int>> &offsets,
    const PiecePlacement &piecePlacement)
{
  std::vector<BoardIndex> res = {};

  const auto [file, rank] = indexToFileRank(index);
  int targetFile, targetRank; // not FileRankIndex since we are testing if on board
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
      const auto targetPiece = piecePlacement[targetIndex];

      if (targetPiece != ChessPiece::Empty)
      {
        if (getPieceColor(targetPiece) != color)
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
    const PieceColor color,
    const std::vector<std::pair<int, int>> &offsets,
    const PiecePlacement &piecePlacement)
{
  std::vector<BoardIndex> res = {};

  const auto [file, rank] = indexToFileRank(index);
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
    const auto targetPiece = piecePlacement[targetIndex];

    if (targetPiece != ChessPiece::Empty && color == getPieceColor(targetPiece))
    {
      continue;
    }

    res.push_back(targetIndex);
  }

  return res;
}

std::vector<BoardIndex> Piece::filterSelfCheckMoves(
    const PiecePlacement &piecePlacement,
    const BoardIndex index,
    const std::vector<BoardIndex> &indexes)
{
  const auto isKingInCheckLambda = [piecePlacement, index](const BoardIndex toIndex)
  {
    const auto color = getPieceColor(piecePlacement[index]);
    auto newPiecePlacement = piecePlacement;
    newPiecePlacement[toIndex] = newPiecePlacement[index];
    newPiecePlacement[index] = ChessPiece::Empty;
    return !Game::isKingInCheck(color, newPiecePlacement);
  };

  std::vector<BoardIndex> legalIndexes;
  std::copy_if(indexes.cbegin(), indexes.cend(), std::back_inserter(legalIndexes), isKingInCheckLambda);

  return legalIndexes;
};

Pawn::Pawn(Game &g) : Piece(g) {}

std::vector<BoardIndex> Pawn::legalSquareIndexes(const BoardIndex index) const
{
  std::vector<BoardIndex> potentialIndexes;

  const auto piece = game.state.piecePlacement[index];
  auto targetPiece = ChessPiece::Empty;
  const auto color = getPieceColor(piece);
  const auto [file, rank] = indexToFileRank(index);
  BoardIndex targetIndex;

  const int sign = color == PieceColor::White ? 1 : -1;
  static const int blackStartRank = 7;
  static const int whiteStartRank = 2;

  // 1 rank
  targetIndex = fileRankToIndex({file, rank + sign});
  if (game.state.piecePlacement[targetIndex] == ChessPiece::Empty)
  {
    potentialIndexes.push_back(targetIndex);
    // 2 ranks
    if ((color == PieceColor::White && rank == whiteStartRank) ||
        (color == PieceColor::Black && rank == blackStartRank))
    {
      targetIndex = fileRankToIndex({file, rank + 2 * sign});
      if (game.state.piecePlacement[targetIndex] == ChessPiece::Empty)
        potentialIndexes.push_back(targetIndex);
    }
  }

  // capture
  const std::array<int, 2> offsets = {1, -1};
  for (auto &offset : offsets)
  {
    const int targetFile = file + offset; // not FileRankIndex since we are testing if on board
    if (targetFile < 1 || 8 < targetFile)
    {
      continue;
    }
    targetIndex = fileRankToIndex({file + offset, rank + sign});
    targetPiece = game.state.piecePlacement[targetIndex];
    if (targetPiece != ChessPiece::Empty && getPieceColor(targetPiece) != color)
      potentialIndexes.push_back(targetIndex);
    // en passant
    if (game.state.enPassantIndex == targetIndex)
      potentialIndexes.push_back(targetIndex);
  }

  const auto legalIndexes = filterSelfCheckMoves(game.state.piecePlacement, index, potentialIndexes);

  return legalIndexes;
}

Knight::Knight(Game &g) : Piece(g) {}

std::vector<BoardIndex> Knight::legalSquareIndexes(const BoardIndex index) const
{
  const auto color = getPieceColor(game.state.piecePlacement[index]);
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

  const auto potentialIndexes = squareIndexes(index, color, offsets, game.state.piecePlacement);

  const auto legalIndexes = filterSelfCheckMoves(game.state.piecePlacement, index, potentialIndexes);

  return legalIndexes;
};

Bishop::Bishop(Game &g) : Piece(g) {}

std::vector<BoardIndex> Bishop::legalSquareIndexes(const BoardIndex index) const
{
  const auto color = getPieceColor(game.state.piecePlacement[index]);
  const std::vector<std::pair<int, int>> offsets = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

  const auto potentialIndexes = linearSquareIndexes(index, color, offsets, game.state.piecePlacement);

  const auto legalIndexes = filterSelfCheckMoves(game.state.piecePlacement, index, potentialIndexes);

  return legalIndexes;
}

Rook::Rook(Game &g) : Piece(g) {}

std::vector<BoardIndex> Rook::legalSquareIndexes(const BoardIndex index) const
{
  const auto color = getPieceColor(game.state.piecePlacement[index]);
  const std::vector<std::pair<int, int>> offsets = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

  const auto potentialIndexes = linearSquareIndexes(index, color, offsets, game.state.piecePlacement);

  const auto legalIndexes = filterSelfCheckMoves(game.state.piecePlacement, index, potentialIndexes);

  return legalIndexes;
}

Queen::Queen(Game &g) : Piece(g) {}

std::vector<BoardIndex> Queen::legalSquareIndexes(const BoardIndex index) const
{
  const auto color = getPieceColor(game.state.piecePlacement[index]);
  const std::vector<std::pair<int, int>> offsets = {
      {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

  const auto potentialIndexes = linearSquareIndexes(index, color, offsets, game.state.piecePlacement);

  const auto legalIndexes = filterSelfCheckMoves(game.state.piecePlacement, index, potentialIndexes);

  return legalIndexes;
}

King::King(Game &g) : Piece(g) {}

std::vector<BoardIndex> King::legalSquareIndexes(const BoardIndex index) const
{
  const auto color = getPieceColor(game.state.piecePlacement[index]);
  const std::vector<std::pair<int, int>> offsets = {
      {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

  auto potentialIndexes = squareIndexes(index, color, offsets, game.state.piecePlacement);

  if (game.state.castlingAvailability.whiteShort && game.state.piecePlacement[61] == ChessPiece::Empty &&
      game.state.piecePlacement[62] == ChessPiece::Empty &&
      !Game::isSquareUnderAttack(61, PieceColor::White, game.state.piecePlacement) &&
      !Game::isSquareUnderAttack(62, PieceColor::White, game.state.piecePlacement))
  {
    potentialIndexes.push_back(62);
  }
  if (game.state.castlingAvailability.whiteLong && game.state.piecePlacement[59] == ChessPiece::Empty &&
      game.state.piecePlacement[58] == ChessPiece::Empty && game.state.piecePlacement[57] == ChessPiece::Empty &&
      !Game::isSquareUnderAttack(59, PieceColor::White, game.state.piecePlacement) &&
      !Game::isSquareUnderAttack(58, PieceColor::White, game.state.piecePlacement))
  {
    potentialIndexes.push_back(58);
  }
  if (game.state.castlingAvailability.blackShort && game.state.piecePlacement[5] == ChessPiece::Empty &&
      game.state.piecePlacement[6] == ChessPiece::Empty &&
      !Game::isSquareUnderAttack(5, PieceColor::Black, game.state.piecePlacement) &&
      !Game::isSquareUnderAttack(6, PieceColor::Black, game.state.piecePlacement))
  {
    potentialIndexes.push_back(6);
  }
  if (game.state.castlingAvailability.blackLong && game.state.piecePlacement[3] == ChessPiece::Empty &&
      game.state.piecePlacement[2] == ChessPiece::Empty && game.state.piecePlacement[1] == ChessPiece::Empty &&
      !Game::isSquareUnderAttack(3, PieceColor::Black, game.state.piecePlacement) &&
      !Game::isSquareUnderAttack(2, PieceColor::Black, game.state.piecePlacement))
  {
    potentialIndexes.push_back(2);
  }

  auto legalIndexes = filterSelfCheckMoves(game.state.piecePlacement, index, potentialIndexes);

  return legalIndexes;
}
