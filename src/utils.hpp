#pragma once

#include "logger.hpp"
#include "types.hpp"

#include <array>
#include <cctype>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>

struct FileRank
{
  FileRankIndex file;
  FileRankIndex rank;

  bool operator==(const FileRank &fr) const
  {
    return file == fr.file && rank == fr.rank;
  }
};

inline bool is_chess_piece(const char c)
{
  static const std::set<char> chess_pieces{'p', 'r', 'n', 'b', 'k', 'q', 'P', 'R', 'N', 'B', 'K', 'Q'};
  return chess_pieces.find(c) != chess_pieces.end();
}

inline char color_to_char(const PieceColor color)
{
  return static_cast<char>(color);
}

inline PieceColor char_to_Color(const char c)
{
  if (c == 'w')
  {
    return PieceColor::White;
  }

  if (c == 'b')
  {
    return PieceColor::Black;
  }

  throw std::invalid_argument("Argument is not a chess piece color.");
}

inline char chessPiece_to_char(const ChessPiece piece)
{
  return static_cast<char>(piece);
}

inline ChessPiece char_to_ChessPiece(const char c)
{
  if (!is_chess_piece(c))
  {
    throw std::invalid_argument("Argument is not a chess piece.");
  }

  std::unordered_map<char, ChessPiece> piece_map = {
      {'p', ChessPiece::BlackPawn},
      {'n', ChessPiece::BlackKnight},
      {'b', ChessPiece::BlackBishop},
      {'r', ChessPiece::BlackRook},
      {'q', ChessPiece::BlackQueen},
      {'k', ChessPiece::BlackKing},
      {'P', ChessPiece::WhitePawn},
      {'N', ChessPiece::WhiteKnight},
      {'B', ChessPiece::WhiteBishop},
      {'R', ChessPiece::WhiteRook},
      {'Q', ChessPiece::WhiteQueen},
      {'K', ChessPiece::WhiteKing},
  };

  return piece_map[c];
}

inline PieceColor piece_color(const ChessPiece piece)
{
  char c_char = chessPiece_to_char(piece);
  return ((std::tolower(c_char) == c_char) ? PieceColor::Black : PieceColor::White);
}

inline BoardIndex algebraic_to_index(const std::string &algebraic_square)
{
  if (algebraic_square.size() != 2)
  {
    throw std::invalid_argument("Argument is not 2 characters long.");
  }

  char file_c = std::tolower(algebraic_square[0]);
  char rank_c = algebraic_square[1];

  FileRankIndex file_i = file_c - 'a' + 1;
  FileRankIndex rank_i = rank_c - '0';

  return BoardIndex(8 * (8 - (rank_i)) + file_i - 1);
}

inline std::string index_to_algebraic(const int index) // need to update this with enpassant
{
  FileRankIndex file_i = index % 8 + 1;
  FileRankIndex rank_i = 8 - (index / 8);

  char file_c = (file_i - 1) + 'a';
  char rank_c = rank_i + '0';

  return std::string(1, file_c) + std::string(1, rank_c);
}

inline PiecePlacement piece_placement_string_to_array(const std::string &s)
{
  PiecePlacement res;
  size_t res_i = 0;

  for (auto &c : s)
  {
    if (res_i == 64)
    {
      throw std::invalid_argument("Array elements exceeds 64.");
      break;
    }

    if (c == '/')
      continue;

    if (isdigit(c))
    {
      for (int i = 0; i != (c - '0'); ++i)
        res[res_i++] = ChessPiece::Empty;
    }
    else
    {
      if (!is_chess_piece(c))
      {
        throw std::invalid_argument("Encountered invalid chess piece.");
      }

      res[res_i++] = char_to_ChessPiece(c);
    }
  }

  if (res_i != 64)
  {
    throw std::invalid_argument("Incorrect number of array elements.");
  }

  return res;
}

inline std::string piece_placement_array_to_string(const PiecePlacement &a)
{
  std::string res;
  unsigned gap = 0;

  auto handle_gap = [&]()
  {
    if (gap > 0)
    {
      res += std::to_string(gap);
      gap = 0;
    }
  };

  ChessPiece piece;
  for (int i = 0; i != 64; i++)
  {
    if (i % 8 == 0 && i != 0)
    {
      handle_gap();
      res += '/';
    }

    piece = a[i];
    if (piece != ChessPiece::Empty)
    {
      handle_gap();
      res += chessPiece_to_char(piece);
    }
    else
      ++gap;
  }
  handle_gap();

  return res;
}

inline CastlingAvailability parse_castling_availability(const std::string &castling_availability_string)
{
  if (castling_availability_string.size() > 4)
  {
    throw std::invalid_argument("Castling availability string must be < 4 chars");
  }

  CastlingAvailability result;

  const std::string valid_chars = "KQkq";

  for (auto &c : castling_availability_string)
  {
    if (valid_chars.find(c) == std::string::npos)
    {
      throw std::invalid_argument("Invaid char in castling availability string");
    }

    switch (c)
    {
    case 'K':
      result.white_short = true;
      break;
    case 'Q':
      result.white_long = true;
      break;
    case 'k':
      result.black_short = true;
      break;
    case 'q':
      result.black_long = true;
      break;
    }
  }

  return result;
}

inline std::string castling_availability_to_string(const CastlingAvailability& castling_availability)
{
  std::string result = "";
  result.reserve(4);

  if (castling_availability.white_short)
  {
    result += "K";
  }
  if (castling_availability.white_long)
  {
    result += "Q";
  }
  if (castling_availability.black_short)
  {
    result += "k";
  }
  if (castling_availability.black_long)
  {
    result += "q";
  }

  return result;
}

inline FileRank index_to_file_rank(const BoardIndex index)
{
  FileRankIndex file = index % 8 + 1;
  FileRankIndex rank = 8 - index / 8;

  return {file, rank};
};

inline int file_rank_to_index(FileRank square)
{
  auto [file, rank] = square;

  int res = (file - 1) + 8 * (8 - rank);
  return res;
};