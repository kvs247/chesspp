#include "game.h"
#include "utils.h"
#include "constants.h"

#include <string>
#include <array>
#include <fstream>

std::string starting_castling_availability = "KQkq";

Game::Game(const std::array<char, 64> &pp, char ac, std::string ca, int ep, int hc, int fc)
    : piece_placement(pp), active_color(ac), castling_availability(ca),
      en_passant_index(ep), halfmove_clock(hc), fullmove_clock(fc) {}

Game::Game() : Game(starting_piece_placement, 'w', starting_castling_availability, -1, 0, 0) {}

std::string Game::get_fen_str()
{
  std::string fen;
  fen += piece_placement_array_to_string(piece_placement);
  fen += ' ';

  fen += active_color;
  fen += ' ';

  fen += castling_availability;
  fen += ' ';

  if (en_passant_index < 0)
    fen += '-';
  else
    fen += index_to_square(en_passant_index);
  fen += ' ';

  fen += std::to_string(halfmove_clock);
  fen += ' ';

  fen += std::to_string(fullmove_clock);
  fen += ' ';

  return fen;
}

std::array<char, 64> Game::get_piece_placement()
{
  return piece_placement;
}

bool Game::move(int from_index, int to_index)
{
  // do chess logic here
  piece_placement[to_index] = piece_placement[from_index];
  piece_placement[from_index] = '\0';

  return true;
}
