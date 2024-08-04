#ifndef PIECE_H
#define PIECE_H

#include <array>
#include <iostream>
#include <vector>

class Game;

class Piece
{
public:
  Piece(Game &);

  virtual std::vector<int> legal_square_indexes(int) = 0; // pure virtual

protected:
  Game &game;
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

#endif