#include "piece.h"
#include "game.h"
#include "logger.h"
#include <iostream>

Piece::Piece(Game &g) : game(g) {}

// Knight
Knight::Knight(Game &g) : Piece(g) {}

bool Knight::is_legal_move(int from_index, int to_index)
{
  return true;
}