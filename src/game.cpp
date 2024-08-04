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
      pawn(*this),
      knight(*this),
      bishop(*this),
      rook(*this),
      queen(*this) {}

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
      if (token == "-")
        en_passant_index = -1;
      else
        en_passant_index = algebraic_to_index(token);
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

  char from_piece = piece_placement[from_index];
  char to_piece = piece_placement[to_index];

  if (from_piece == '\0')
    return false;
  char from_color = piece_color(from_piece);

  char to_color = '\0';
  if (to_piece)
    to_color = piece_color(to_piece);

  if (from_color == to_color)
    return false;

  std::vector<int> indexes = {};

  switch (std::tolower(from_piece))
  {
  case 'p':
    indexes = pawn.legal_square_indexes(from_index);
    if (std::find(indexes.begin(), indexes.end(), to_index) == indexes.end())
      return false;
    break;
  case 'n':
    indexes = knight.legal_square_indexes(from_index);
    if (std::find(indexes.begin(), indexes.end(), to_index) == indexes.end()) // do this find after the switch when all piece logic is in
      return false;
    break;
  case 'b':
    indexes = bishop.legal_square_indexes(from_index);
    if (std::find(indexes.begin(), indexes.end(), to_index) == indexes.end())
      return false;
    break;
  case 'r':
    indexes = rook.legal_square_indexes(from_index);
    if (std::find(indexes.begin(), indexes.end(), to_index) == indexes.end())
      return false;
    break;
  case 'q':
    indexes = queen.legal_square_indexes(from_index);
    if (std::find(indexes.begin(), indexes.end(), to_index) == indexes.end())
      return false;
    break;
  default:
    break;
  }

  piece_placement[to_index] = piece_placement[from_index];
  piece_placement[from_index] = '\0';

  handle_en_passant(from_piece, from_color, from_index, to_index);

  return true;
}

void Game::read_move(int &from_index, int &to_index) const
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

void Game::handle_en_passant(char from_piece, char from_color, int from_index, int to_index)
{
  bool is_pawn = std::tolower(from_piece) == 'p';
  if (to_index == en_passant_index)
    piece_placement[to_index + (from_color == 'w' ? +8 : -8)] = '\0';

  if (is_pawn && abs(from_index - to_index) == 16)
    en_passant_index = from_index + (from_color == 'w' ? -8 : +8);
  else
    en_passant_index = -1;
}
