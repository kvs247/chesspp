#pragma once

#include <array>
#include <iostream>
#include <vector>

class Game;

class Piece
{
public:
  Piece(Game &);

  virtual std::vector<int> legal_square_indexes(int) = 0; // pure virtual

  static std::vector<int> linear_square_indexes(
      int,
      const std::vector<std::pair<int, int>> &,
      PiecePlacement &);

  static std::vector<int> square_indexes(int, const std::vector<std::pair<int, int>> &);

protected:
  Game &game;
};

struct Pawn : public Piece
{
  Pawn(Game &);
  std::vector<int> legal_square_indexes(int) override;
};

struct Knight : public Piece
{
  Knight(Game &);
  std::vector<int> legal_square_indexes(int) override;
};

struct Bishop : public Piece
{
  Bishop(Game &);
  std::vector<int> legal_square_indexes(int) override;
};

struct Rook : public Piece
{
  Rook(Game &);
  std::vector<int> legal_square_indexes(int) override;
};

struct Queen : public Piece
{
  Queen(Game &);
  std::vector<int> legal_square_indexes(int) override;
};

struct King : public Piece
{
  King(Game &);
  std::vector<int> legal_square_indexes(int) override;
};
