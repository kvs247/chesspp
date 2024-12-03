#pragma once

#include <utility>
#include <vector>

#include "types.hpp"

class Game;

class Piece
{
public:
  Piece(Game &);

  virtual std::vector<BoardIndex> legalSquareIndexes(const BoardIndex) const = 0; // pure virtual

  static std::vector<BoardIndex> linearSquareIndexes(
      const BoardIndex,
      const PieceColor,
      const std::vector<std::pair<int, int>> &,
      const PiecePlacement &);

  static std::vector<BoardIndex>
  squareIndexes(const BoardIndex, const PieceColor, const std::vector<std::pair<int, int>> &, const PiecePlacement &);

  static std::vector<BoardIndex>
  filterSelfCheckMoves(const PiecePlacement &, const BoardIndex, const std::vector<BoardIndex> &);

protected:
  Game &game;
};

struct Pawn : public Piece
{
  Pawn(Game &);
  std::vector<BoardIndex> legalSquareIndexes(const BoardIndex) const override;
};

struct Knight : public Piece
{
  Knight(Game &);
  std::vector<BoardIndex> legalSquareIndexes(const BoardIndex) const override;
};

struct Bishop : public Piece
{
  Bishop(Game &);
  std::vector<BoardIndex> legalSquareIndexes(const BoardIndex) const override;
};

struct Rook : public Piece
{
  Rook(Game &);
  std::vector<BoardIndex> legalSquareIndexes(const BoardIndex) const override;
};

struct Queen : public Piece
{
  Queen(Game &);
  std::vector<BoardIndex> legalSquareIndexes(const BoardIndex) const override;
};

struct King : public Piece
{
  King(Game &);
  std::vector<BoardIndex> legalSquareIndexes(const BoardIndex) const override;
};
