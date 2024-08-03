#ifndef PIECE_H
#define PIECE_H

#include <iostream>

class Game;

class Piece
{
public:
  Piece(Game &);

  virtual bool is_legal_move(int, int) = 0; // pure virtual

protected:
  Game &game;
};

struct Knight : public Piece
{
  Knight(Game &);
  bool is_legal_move(int, int) override;
};

#endif