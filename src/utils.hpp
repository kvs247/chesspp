#pragma once

#include <cctype>
#include <optional>
#include <set>
#include <stddef.h>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "types.hpp"

struct FileRank
{
  FileRankIndex file;
  FileRankIndex rank;

  bool operator==(const FileRank &fr) const { return file == fr.file && rank == fr.rank; }
};

inline bool isChessPiece(const char c)
{
  static const std::set<char> chessPieces{'p', 'r', 'n', 'b', 'k', 'q', 'P', 'R', 'N', 'B', 'K', 'Q'};
  return chessPieces.find(c) != chessPieces.end();
}

inline char colorToChar(const PieceColor color) { return static_cast<char>(color); }

inline PieceColor charToColor(const char c)
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

inline char chessPieceToChar(const ChessPiece piece) { return static_cast<char>(piece); }

inline ChessPiece charToChessPiece(const char c)
{
  if (!isChessPiece(c))
  {
    throw std::invalid_argument("Argument is not a chess piece.");
  }

  static const std::unordered_map<char, ChessPiece> pieceMap = {
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

  return pieceMap.at(c);
}

inline PieceColor getPieceColor(const ChessPiece piece)
{
  if (piece == ChessPiece::Empty)
  {
    throw std::invalid_argument("pieceColor: input is an empty piece");
  }

  const char cChar = chessPieceToChar(piece);
  return ((std::tolower(cChar) == cChar) ? PieceColor::Black : PieceColor::White);
}

inline BoardIndex algebraicToIndex(const std::string &algebraicSquare)
{
  if (algebraicSquare.size() != 2)
  {
    throw std::invalid_argument("Argument is not 2 characters long.");
  }

  const char fileChar = std::tolower(algebraicSquare[0]);
  const char rankChar = algebraicSquare[1];

  const FileRankIndex file = fileChar - 'a' + 1;
  const FileRankIndex rank = rankChar - '0';

  return BoardIndex(8 * (8 - (rank)) + file - 1);
}

inline std::string indexToAlgebraic(const std::optional<BoardIndex> index)
{
  if (!index)
  {
    return "-";
  }

  const FileRankIndex file = index.value() % 8 + 1;
  const FileRankIndex rank = 8 - (index.value() / 8);

  char fileChar = (file - 1) + 'a';
  char rankChar = rank + '0';
  return std::string(1, fileChar) + std::string(1, rankChar);
}

inline PiecePlacement piecePlacementStringToArray(const std::string &s)
{
  PiecePlacement res;
  size_t resIdx = 0;

  for (auto &c : s)
  {
    if (resIdx == 64)
    {
      throw std::invalid_argument("Array elements exceeds 64.");
      break;
    }

    if (c == '/')
      continue;

    if (isdigit(c))
    {
      for (int i = 0; i != (c - '0'); ++i)
        res[resIdx++] = ChessPiece::Empty;
    }
    else
    {
      if (!isChessPiece(c))
      {
        throw std::invalid_argument("Encountered invalid chess piece.");
      }

      res[resIdx++] = charToChessPiece(c);
    }
  }

  if (resIdx != 64)
  {
    throw std::invalid_argument("Incorrect number of array elements.");
  }

  return res;
}

inline std::string piecePlacementArrayToString(const PiecePlacement &a)
{
  std::string res;
  unsigned gap = 0;

  const auto handleGap = [&]()
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
      handleGap();
      res += '/';
    }

    piece = a[i];
    if (piece != ChessPiece::Empty)
    {
      handleGap();
      res += chessPieceToChar(piece);
    }
    else
      ++gap;
  }
  handleGap();

  return res;
}

inline CastlingAvailability parseCastlingAvailability(const std::string &castlingAvailabilityString)
{
  if (castlingAvailabilityString.size() > 4)
  {
    throw std::invalid_argument("Castling availability string must be < 4 chars");
  }

  CastlingAvailability result({false, false, false, false});

  if (castlingAvailabilityString == "-")
  {
    return result;
  }

  const std::string validChars = "KQkq";

  for (auto &c : castlingAvailabilityString)
  {
    if (validChars.find(c) == std::string::npos)
    {
      throw std::invalid_argument("Invaid char in castling availability string");
    }

    switch (c)
    {
    case 'K':
      result.whiteShort = true;
      break;
    case 'Q':
      result.whiteLong = true;
      break;
    case 'k':
      result.blackShort = true;
      break;
    case 'q':
      result.blackLong = true;
      break;
    }
  }

  return result;
}

inline std::string castlingAvailabilityToString(const CastlingAvailability &castlingAvailability)
{
  std::string result = "";
  result.reserve(4);

  if (castlingAvailability.whiteShort)
  {
    result += "K";
  }
  if (castlingAvailability.whiteLong)
  {
    result += "Q";
  }
  if (castlingAvailability.blackShort)
  {
    result += "k";
  }
  if (castlingAvailability.blackLong)
  {
    result += "q";
  }

  if (result.empty())
  {
    result = "-";
  }

  return result;
}

inline FileRank indexToFileRank(const BoardIndex index)
{
  const FileRankIndex file = index % 8 + 1;
  const FileRankIndex rank = 8 - index / 8;

  return {file, rank};
};

inline int fileRankToIndex(FileRank square)
{
  const auto [file, rank] = square;
  const int res = (file - 1) + 8 * (8 - rank);
  return res;
};
