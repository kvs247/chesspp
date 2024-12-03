#pragma once

#include <array>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <vector>

enum class PieceColor : char
{
  White = 'w',
  Black = 'b',

};

inline PieceColor operator!(const PieceColor current)
{
  return (current == PieceColor::White) ? PieceColor::Black : PieceColor::White;
}

enum class ChessPiece : char
{
  BlackPawn = 'p',
  BlackKnight = 'n',
  BlackBishop = 'b',
  BlackRook = 'r',
  BlackQueen = 'q',
  BlackKing = 'k',
  WhitePawn = 'P',
  WhiteKnight = 'N',
  WhiteBishop = 'B',
  WhiteRook = 'R',
  WhiteQueen = 'Q',
  WhiteKing = 'K',
  Empty = '\0',
};

using PiecePlacement = std::array<ChessPiece, 64>;

struct CastlingAvailability
{
  bool whiteShort;
  bool whiteLong;
  bool blackShort;
  bool blackLong;

  bool operator==(const CastlingAvailability &other) const
  {
    return (
        whiteShort == other.whiteShort && whiteLong == other.whiteLong && blackShort == other.blackShort &&
        blackLong == other.blackLong);
  }
};

class RangedInt
{
private:
  int val;
  int minVal;
  int maxVal;

  void validate(int v)
  {
    if (v < minVal || v > maxVal)
    {
      throw std::out_of_range(
          "RangedInt must be in range [" + std::to_string(minVal) + ", " + std::to_string(maxVal) + "]\n");
    }
  }

public:
  RangedInt() = default;

  explicit RangedInt(int v, int minV, int maxV) : minVal(minV), maxVal(maxV)
  {
    validate(v);
    val = v;
  }

  operator int() const { return val; }

  RangedInt &operator=(int v)
  {
    validate(v);
    val = v;
    return *this;
  }

  RangedInt &operator+=(int v)
  {
    validate(val + v);
    val += v;
    return *this;
  }

  RangedInt &operator++()
  {
    validate(val + 1);
    val += 1;
    return *this;
  }
};

class BoardIndex : public RangedInt
{
public:
  BoardIndex() : RangedInt(0, 0, 63) {}

  BoardIndex(int v) : RangedInt(v, 0, 63) {}

  static std::vector<BoardIndex> create_vector(std::initializer_list<int> indices)
  {
    std::vector<BoardIndex> result;
    result.reserve(indices.size());
    for (auto &index : indices)
    {
      result.emplace_back(BoardIndex(index));
    }

    return result;
  }
};

class FileRankIndex : public RangedInt
{
public:
  FileRankIndex() : RangedInt(1, 1, 8) {}

  FileRankIndex(int v) : RangedInt(v, 1, 8) {}
};

struct MoveListItem
{
  BoardIndex fromIndex;
  ChessPiece fromPiece;
  BoardIndex toIndex;
  ChessPiece toPiece;
  std::vector<BoardIndex> samePieceIndexes;
  ChessPiece promotionPiece;
  std::string castlingString;
  bool isOpponentInCheck;
  bool isEnPassantCapture;
};
