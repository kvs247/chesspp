#pragma once

#include "constants.hpp"
#include "logger.hpp"
#include "types.hpp"
#include "utils.hpp"
#include "piece.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
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
  Game(const PiecePlacement &, char, std::string, int, int, int);
  Game();
  Game(std::string &);

  std::string get_fen_str();
  PiecePlacement get_piece_placement();

  bool move();
  void read_move(int &, int &) const;
  void handle_en_passant(char, char, int, int); // could this be private?
  static bool is_king_in_check(char, PiecePlacement &);

private:
  PiecePlacement piece_placement;
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

inline Game::Game(const PiecePlacement &pp, char ac, std::string ca, int ep, int hc, int fc)
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
      queen(*this),
      king(*this) {}

inline Game::Game() : Game(starting_piece_placement, 'w', starting_castling_availability, -1, 0, 0) {}

inline Game::Game(std::string &fen) : Game()
{
  size_t pos = 0;
  std::string token;
  for (int token_count = 1; token_count != 7; ++token_count)
  {
    pos = fen.find(' ');
    token = fen.substr(0, pos);
    fen.erase(0, pos + 1);

    // logger.log(token);

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

inline std::string Game::get_fen_str()
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

inline PiecePlacement Game::get_piece_placement()
{
  return piece_placement;
}

inline bool Game::move()
{
  int from_index, to_index;
  read_move(from_index, to_index);

  char from_piece = piece_placement[from_index];
  char to_piece = piece_placement[to_index];

  if (from_piece == '\0')
  {
    return false;
  }

  char from_color = piece_color(from_piece);
  char to_color = '\0';

  if (to_piece)
  {
    to_color = piece_color(to_piece);
  }

  if (from_color == to_color)
  {
    return false;
  }

  std::vector<int> indexes = {};
  switch (std::tolower(from_piece))
  {
  case 'p':
    indexes = pawn.legal_square_indexes(from_index);
    break;
  case 'n':
    indexes = knight.legal_square_indexes(from_index);
    break;
  case 'b':
    indexes = bishop.legal_square_indexes(from_index);
    break;
  case 'r':
    indexes = rook.legal_square_indexes(from_index);
    break;
  case 'q':
    indexes = queen.legal_square_indexes(from_index);
    break;
  case 'k':
    indexes = king.legal_square_indexes(from_index);
    break;
  default:
    break;
  }

  if (std::find(indexes.begin(), indexes.end(), to_index) == indexes.end())
  {

    return false;
  }

  PiecePlacement new_piece_placement = piece_placement;
  new_piece_placement[to_index] = piece_placement[from_index];
  new_piece_placement[from_index] = '\0';
  if (is_king_in_check(from_color, new_piece_placement))
  {
    return false;
  };

  piece_placement = new_piece_placement;

  handle_en_passant(from_piece, from_color, from_index, to_index);

  return true;
}

inline void Game::read_move(int &from_index, int &to_index) const
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

inline void Game::handle_en_passant(char from_piece, char from_color, int from_index, int to_index)
{
  bool is_pawn = std::tolower(from_piece) == 'p';
  if (to_index == en_passant_index)
    piece_placement[to_index + (from_color == 'w' ? +8 : -8)] = '\0';

  if (is_pawn && abs(from_index - to_index) == 16)
    en_passant_index = from_index + (from_color == 'w' ? -8 : +8);
  else
    en_passant_index = -1;
}

inline bool Game::is_king_in_check(char color, PiecePlacement &piece_placement)
{
  char king_piece = color == 'w' ? 'K' : 'k';
  auto king_iter = std::find(piece_placement.begin(), piece_placement.end(), king_piece);
  int king_index = std::distance(piece_placement.begin(), king_iter);

  auto piece_in_indexes = [color, piece_placement](char piece, std::vector<int> &indexes)
  {
    for (auto &idx : indexes)
    {
      char idx_piece = piece_placement[idx];
      if (!idx_piece || piece_color(idx_piece) == color)
      {
        continue;
      }

      if (std::tolower(idx_piece) == std::tolower(piece))
      {
        return true;
      }

      if (std::tolower(piece) == 'b' || std::tolower(piece) == 'r')
      {
        if (std::towlower(idx_piece) == 'q')
        {
          return true;
        }
      }
    }
    return false;
  };

  // pawn
  const std::vector<std::pair<int, int>> pawn_offsets =
      (color == 'w')
          ? std::vector<std::pair<int, int>>{
                {1, 1},
                {-1, 1},
            }
          : std::vector<std::pair<int, int>>{
                {1, -1},
                {-1, -1},
            };
  std::vector<int> pawn_indexes = Piece::square_indexes(king_index, pawn_offsets);
  if (piece_in_indexes('p', pawn_indexes))
  {
    return true;
  }

  // knight
  const std::vector<std::pair<int, int>> knight_offsets = {
      {1, 2},
      {1, -2},
      {-1, 2},
      {-1, -2},
      {2, 1},
      {2, -1},
      {-2, 1},
      {-2, -1},
  };
  auto knight_indexes = Piece::square_indexes(king_index, knight_offsets);
  if (piece_in_indexes('n', knight_indexes))
  {
    return true;
  }

  // bishop/queen
  const std::vector<std::pair<int, int>> diag_offsets = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
  auto diag_indexes = Piece::linear_square_indexes(king_index, diag_offsets, piece_placement);
  if (piece_in_indexes('b', diag_indexes))
  {
    return true;
  }

  // rook/queen
  const std::vector<std::pair<int, int>> horizvert_offsets = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
  auto horizvert_indexes = Piece::linear_square_indexes(king_index, horizvert_offsets, piece_placement);
  if (piece_in_indexes('r', horizvert_indexes))
  {
    return true;
  }

  // king
  const std::vector<std::pair<int, int>> king_offsets = {
      {1, 0},
      {-1, 0},
      {0, 1},
      {0, -1},
      {1, 1},
      {1, -1},
      {-1, 1},
      {-1, -1},
  };
  auto king_indexes = Piece::square_indexes(king_index, king_offsets);
  if (piece_in_indexes('k', king_indexes))
  {
    return true;
  }

  return false;
}
