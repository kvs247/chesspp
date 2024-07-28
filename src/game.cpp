#include "game.h"
#include "utils.h"
#include "constants.h"

#include <string>
#include <array>
#include <fstream>

std::string starting_castling_availability = "KQkq";

Game::Game(std::array<char, 64> &pp, char ac, std::string ca, int ep, int hc, int fc)
    : piece_placement(pp), active_color(ac), castling_availability(ca),
      en_passant_index(ep), halfmove_clock(hc), fullmove_clock(fc) {}

Game::Game() : Game(starting_piece_placement, 'w', starting_castling_availability, -1, 0, 0) {}

std::string Game::get_fen_str()
{
  // piece position
  std::string fen;
  unsigned gap = 0;

  auto handle_gap = [&]()
  {
    if (gap > 0)
    {
      fen += std::to_string(gap);
      gap = 0;
    }
  };

  for (int i = 0; i != 64; i++)
  {
    if (i % 8 == 0 && i != 0)
    {
      handle_gap();
      fen += '/';
    }
    char c = piece_placement[i];
    if (c)
    {
      handle_gap();
      fen += c;
    }
    else
      ++gap;
  }
  handle_gap();
  fen += ' ';

  // active color
  fen += active_color;
  fen += ' ';

  // castling availability
  fen += castling_availability;
  fen += ' ';

  // en passant square
  if (en_passant_index < 0)
    fen += '-';
  else
    fen += index_to_square(en_passant_index);
  fen += ' ';

  // halfmove clock
  fen += std::to_string(halfmove_clock);
  fen += ' ';

  // fullmove check
  fen += std::to_string(fullmove_clock);
  fen += ' ';

  return fen;
}
