#include "game.h"
#include "utils.h"
#include "constants.h"
#include "move.h"

#include <iostream>
#include <string>
#include <array>
#include <fstream>
#include <sstream>
#include <cctype>

std::string starting_castling_availability = "KQkq";

Game::Game(const std::array<char, 64> &pp, char ac, std::string ca, int ep, int hc, int fc)
    : piece_placement(pp), active_color(ac), castling_availability(ca),
      en_passant_index(ep), halfmove_clock(hc), fullmove_clock(fc) {}

Game::Game() : Game(starting_piece_placement, 'w', starting_castling_availability, -1, 0, 0) {}

std::string Game::get_fen_str()
{
  std::ostringstream fen;
  std::string en_passant =
      (en_passant_index < 0) ? "-" : index_to_algebraic(en_passant_index);

  fen << piece_placement_array_to_string(piece_placement) << " "
      << active_color << " "
      << castling_availability << " "
      << en_passant << " "
      << std::to_string(halfmove_clock) << " "
      << std::to_string(fullmove_clock);

  return fen.str();
}

std::array<char, 64> Game::get_piece_placement()
{
  return piece_placement;
}

bool Game::move()
{
  int from_index, to_index;
  read_move(from_index, to_index);

  char piece = piece_placement[from_index];
  switch (std::tolower(piece))
  {
  case '\0':
    return false;
  case 'n':
    return knight(from_index, to_index);
  }

  piece_placement[to_index] = piece_placement[from_index];
  piece_placement[from_index] = '\0';

  return true;
}

void Game::read_move(int &from_index, int &to_index)
{
  std::string from_algebraic, to_algebraic;

  std::cout << "from square: ";
  std::cin >> from_algebraic;
  std::cout << "\n";
  from_index = algebraic_to_index(from_algebraic);

  std::cout << "to square: ";
  std::cin >> to_algebraic;
  std::cout << "\n";
  to_index = algebraic_to_index(to_algebraic);
}
