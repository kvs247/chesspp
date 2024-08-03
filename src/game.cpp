#include "constants.h"
#include "game.h"
#include "logger.h"
#include "utils.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

Game::Game(const std::array<char, 64> &pp, char ac, std::string ca, int ep, int hc, int fc)
    : piece_placement(pp),
      active_color(ac),
      castling_availability(ca),
      en_passant_index(ep),
      halfmove_clock(hc),
      fullmove_clock(fc),
      knight(*this) {}

Game::Game() : Game(starting_piece_placement, 'w', starting_castling_availability, -1, 0, 0) {}

Game::Game(std::string &fen) : Game()
{
  size_t pos = 0;
  std::string token;
  for (int token_count = 1; token_count != 7; ++token_count)
  {
    pos = fen.find(' ');
    token = fen.substr(0, pos);
    fen.erase(0, pos + 1);

    logger.log(token);

    switch (token_count)
    {
    case 1:
      piece_placement = piece_placement_string_to_array(token);
      break;
    case 2:
      active_color = token[0];
      break;
    case 3:
      castling_availability = token;
      break;
    case 4:
      en_passant_index = std::stoi(token == "-" ? "-1" : token);
      break;
    case 5:
      halfmove_clock = std::stoi(token);
      break;
    case 6:
      fullmove_clock = std::stoi(token);
      break;
    default:
      break;
    }
  }
}

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

  std::vector<int> indexes = {};
  char piece = piece_placement[from_index];
  switch (std::tolower(piece))
  {
  case '\0':
    return false;
  case 'n':
    indexes = knight.legal_square_indexes(from_index);
    if (std::find(indexes.begin(), indexes.end(), to_index) == indexes.end())
      return false;
    break;
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
