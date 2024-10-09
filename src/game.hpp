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

#include <cstdlib>
#include <ctime>
#include <vector>
#include <sstream>

#define BLACK_IS_CPU true
#define DISABLE_TURN_ORDER true

class Game
{
  friend class Piece;
  friend class Pawn;
  friend class Bishop;
  friend class Rook;
  friend class Queen;
  friend class King;

public:
  Game(const PiecePlacement &, PieceColor, std::string, int, int, int);
  Game();
  Game(std::string &);

  std::string get_fen_str();
  PiecePlacement get_piece_placement();

  bool move();
  void read_move(BoardIndex &, BoardIndex &) const;
  std::vector<BoardIndex> get_piece_legal_moves(const ChessPiece &, const BoardIndex) const;
  void handle_en_passant(const ChessPiece, const PieceColor, const BoardIndex, const BoardIndex); // could this be private?
  std::pair<BoardIndex, BoardIndex> generate_cpu_move_black() const;
  static bool is_king_in_check(const PieceColor, const PiecePlacement &);

private:
  PiecePlacement piece_placement;
  PieceColor active_color;
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

inline Game::Game(
    const PiecePlacement &pp,
    PieceColor ac,
    std::string ca,
    int ep,
    int hc,
    int fc)
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

inline Game::Game() : Game(starting_piece_placement, PieceColor::White, starting_castling_availability, -1, 0, 0) {}

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
      active_color = char_to_Color(token[0]);
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
      << color_to_char(active_color) << " "
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
  BoardIndex from_index, to_index;

  if (BLACK_IS_CPU && active_color == PieceColor::Black)
  {
    auto [cpu_from_index, cpu_to_index] = generate_cpu_move_black();
    from_index = cpu_from_index;
    to_index = cpu_to_index;
    // logger.log("to: ", to_index, " from: ", from_index);
  }
  else
  {
    read_move(from_index, to_index);
  }

  auto from_piece = piece_placement[from_index];
  auto to_piece = piece_placement[to_index];

  if (from_piece == ChessPiece::Empty)
  {
    return false;
  }

  auto from_color = piece_color(from_piece);
  if (!DISABLE_TURN_ORDER && from_color != active_color)
  {
    return false;
  }

  PieceColor to_color;
  if (to_piece != ChessPiece::Empty)
  {
    to_color = piece_color(to_piece);
  }

  if (from_color == to_color)
  {
    return false;
  }

  auto indexes = get_piece_legal_moves(from_piece, from_index);
  if (std::find(indexes.begin(), indexes.end(), to_index) == indexes.end())
  {
    return false;
  }

  PiecePlacement new_piece_placement = piece_placement;
  new_piece_placement[to_index] = piece_placement[from_index];
  new_piece_placement[from_index] = ChessPiece::Empty;
  if (is_king_in_check(from_color, new_piece_placement))
  {
    return false;
  };

  piece_placement = new_piece_placement;

  active_color = !active_color;

  handle_en_passant(from_piece, from_color, from_index, to_index);

  // logger.log(this->get_fen_str());

  return true;
}

inline void Game::read_move(BoardIndex &from_index, BoardIndex &to_index) const
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

inline std::vector<BoardIndex> Game::get_piece_legal_moves(
    const ChessPiece &piece,
    const BoardIndex index) const
{
  std::vector<BoardIndex> indexes = {};
  switch (piece)
  {
  case ChessPiece::BlackPawn:
  case ChessPiece::WhitePawn:
    indexes = pawn.legal_square_indexes(index);
    break;
  case ChessPiece::BlackKnight:
  case ChessPiece::WhiteKnight:
    indexes = knight.legal_square_indexes(index);
    break;
  case ChessPiece::BlackBishop:
  case ChessPiece::WhiteBishop:
    indexes = bishop.legal_square_indexes(index);
    break;
  case ChessPiece::BlackRook:
  case ChessPiece::WhiteRook:
    indexes = rook.legal_square_indexes(index);
    break;
  case ChessPiece::BlackQueen:
  case ChessPiece::WhiteQueen:
    indexes = queen.legal_square_indexes(index);
    break;
  case ChessPiece::BlackKing:
  case ChessPiece::WhiteKing:
    indexes = king.legal_square_indexes(index);
    break;
  default:
    break;
  }

  return indexes;
}

inline void Game::handle_en_passant(
    const ChessPiece from_piece,
    const PieceColor from_color,
    const BoardIndex from_index,
    const BoardIndex to_index)
{
  bool is_pawn = (from_piece == ChessPiece::BlackPawn || from_piece == ChessPiece::WhitePawn);
  if (to_index == en_passant_index)
    piece_placement[to_index + (from_color == PieceColor::White ? +8 : -8)] = ChessPiece::Empty;

  if (is_pawn && abs(from_index - to_index) == 16)
    en_passant_index = from_index + (from_color == PieceColor::White ? -8 : +8);
  else
    en_passant_index = -1;
}

inline std::pair<BoardIndex, BoardIndex> Game::generate_cpu_move_black() const
{
  std::vector<int> cpu_pieces_idxs;
  cpu_pieces_idxs.reserve(32);

  for (size_t i = 0; i < piece_placement.size(); ++i)
  {
    auto piece = piece_placement[i];
    if (piece != ChessPiece::Empty && piece_color(piece_placement[i]) == PieceColor::Black)
    {
      cpu_pieces_idxs.push_back(i);
    }
  }
  std::random_shuffle(cpu_pieces_idxs.begin(), cpu_pieces_idxs.end());

  // std::srand(std::time(nullptr)); // may want to do this at program start

  BoardIndex res_from_index, res_to_index;

  bool stop = false;
  for (auto from_index : cpu_pieces_idxs)
  {
    logger.log(from_index);
    if (stop)
    {
      break;
    }

    auto from_piece = piece_placement[from_index];
    auto indexes = get_piece_legal_moves(from_piece, from_index);
    if (!indexes.size())
    {
      continue;
    }

    std::random_shuffle(indexes.begin(), indexes.end());

    for (auto to_index : indexes)
    {
      auto to_piece = piece_placement[to_index];
      if (to_piece != ChessPiece::Empty && (piece_color(from_piece) == piece_color(to_piece)))
      {
        continue;
      }

      PiecePlacement new_piece_placement = piece_placement;
      new_piece_placement[to_index] = new_piece_placement[from_index];
      new_piece_placement[from_index] = ChessPiece::Empty;
      if (is_king_in_check(piece_color(from_piece), new_piece_placement))
      {
        continue;
      }

      stop = true;
      res_from_index = from_index;
      res_to_index = to_index;
      break;
    }
  }

  return {res_from_index, res_to_index};
};

inline bool Game::is_king_in_check(
    const PieceColor color,
    const PiecePlacement &piece_placement)
{
  ChessPiece king_piece = color == PieceColor::White ? ChessPiece::WhiteKing : ChessPiece::BlackKing;
  auto king_iter = std::find(piece_placement.cbegin(), piece_placement.cend(), king_piece);
  BoardIndex king_index(king_iter - piece_placement.cbegin());

  auto is_piece_in_indexes_lambda = [color, piece_placement](ChessPiece search_piece, std::vector<BoardIndex> &indexes)
  {
    for (auto &idx : indexes)
    {
      auto current_piece = piece_placement[idx];
      if (current_piece == ChessPiece::Empty || piece_color(current_piece) == color)
      {
        continue;
      }

      char idx_piece_char = chessPiece_to_char(current_piece);
      char piece_char = chessPiece_to_char(search_piece);

      if (std::tolower(idx_piece_char) == std::tolower(piece_char))
      {
        return true;
      }

      if (
          search_piece == ChessPiece::BlackBishop || search_piece == ChessPiece::WhiteBishop ||
          search_piece == ChessPiece::BlackRook || search_piece == ChessPiece::WhiteRook)
      {
        if (current_piece == ChessPiece::BlackQueen || current_piece == ChessPiece::WhiteQueen)
        {
          return true;
        }
      }
    }
    return false;
  };

  // pawn
  const std::vector<std::pair<int, int>> pawn_offsets =
      (color == PieceColor::White)
          ? std::vector<std::pair<int, int>>{
                {1, 1},
                {-1, 1},
            }
          : std::vector<std::pair<int, int>>{
                {1, -1},
                {-1, -1},
            };
  std::vector<BoardIndex> pawn_indexes = Piece::square_indexes(king_index, pawn_offsets);
  if (is_piece_in_indexes_lambda(ChessPiece::BlackPawn, pawn_indexes))
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
  if (is_piece_in_indexes_lambda(ChessPiece::BlackKnight, knight_indexes))
  {
    return true;
  }

  // bishop/queen
  const std::vector<std::pair<int, int>> diag_offsets = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
  auto diag_indexes = Piece::linear_square_indexes(king_index, diag_offsets, piece_placement);
  if (is_piece_in_indexes_lambda(ChessPiece::BlackBishop, diag_indexes))
  {
    return true;
  }

  // rook/queen
  const std::vector<std::pair<int, int>> horizvert_offsets = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
  auto horizvert_indexes = Piece::linear_square_indexes(king_index, horizvert_offsets, piece_placement);
  if (is_piece_in_indexes_lambda(ChessPiece::BlackRook, horizvert_indexes))
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
  if (is_piece_in_indexes_lambda(ChessPiece::BlackKing, king_indexes))
  {
    return true;
  }

  return false;
}
