#ifndef CHESS_H
#define CHESS_H

#include <string>
#include <array>

class Chess
{
public:
  Chess(std::array<char, 64>&, char, std::string, int, int, int);
  Chess();

  std::string get_fen_str();

private:
  std::array<char, 64> piece_placement;
  char active_color;
  std::string castling_availability;
  int en_passant_index;
  int halfmove_clock;
  int fullmove_clock;
};

#endif