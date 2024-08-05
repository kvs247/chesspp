#ifndef GAME_H
#define GAME_H

#include "piece.h"

#include <array>
#include <string>

class Game
{
  friend class Piece;
  friend class Pawn;
  friend class Bishop;
  friend class Rook;
  friend class Queen;
  friend class King;

public:
  Game(const std::array<char, 64> &, char, std::string, int, int, int);
  Game();
  Game(std::string &);

  std::string get_fen_str();
  std::array<char, 64> get_piece_placement();

  bool move();
  void read_move(int &, int &) const;
  void handle_en_passant(char, char, int, int); // could this be private?

private:
  std::array<char, 64> piece_placement;
  char active_color;
  std::string castling_availability;
  int en_passant_index;
  int halfmove_clock;
  int fullmove_clock;

  Pawn pawn;
  Knight knight;
  Bishop bishop;
  Rook rook;
  Queen queen;
  King king;
};

#endif