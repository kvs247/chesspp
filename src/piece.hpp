#pragma once

#include <array>
#include <iostream>
#include <vector>

class Game;

class Piece
{
public:
  Piece(Game &);

  virtual std::vector<BoardIndex> legal_square_indexes(const BoardIndex) = 0; // pure virtual

  static std::vector<BoardIndex> linear_square_indexes(
      const BoardIndex, const std::vector<std::pair<int, int>> &, const PiecePlacement &);

  static std::vector<BoardIndex> square_indexes(
      const BoardIndex, const std::vector<std::pair<int, int>> &);

protected:
  Game &game;
};

struct Pawn : public Piece
{
  Pawn(Game &);
  std::vector<BoardIndex> legal_square_indexes(const BoardIndex) override;
};

struct Knight : public Piece
{
  Knight(Game &);
  std::vector<BoardIndex> legal_square_indexes(const BoardIndex) override;
};

struct Bishop : public Piece
{
  Bishop(Game &);
  std::vector<BoardIndex> legal_square_indexes(const BoardIndex) override;
};

struct Rook : public Piece
{
  Rook(Game &);
  std::vector<BoardIndex> legal_square_indexes(const BoardIndex) override;
};

struct Queen : public Piece
{
  Queen(Game &);
  std::vector<BoardIndex> legal_square_indexes(const BoardIndex) override;
};

struct King : public Piece
{
  King(Game &);
  std::vector<BoardIndex> legal_square_indexes(const BoardIndex) override;
};
