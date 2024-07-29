#ifndef GAME_H
#define GAME_H

#include <string>
#include <array>

class Game
{
public:
  Game(const std::array<char, 64>&, char, std::string, int, int, int);
  Game();

  std::string get_fen_str();
  std::array<char, 64> get_piece_placement();

  bool move(int, int);

private:
  std::array<char, 64> piece_placement;
  char active_color;
  std::string castling_availability;
  int en_passant_index;
  int halfmove_clock;
  int fullmove_clock;
};

#endif