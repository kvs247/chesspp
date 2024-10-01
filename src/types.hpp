#pragma once

#include "logger.hpp"

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

class RangedInt
{
private:
  int val;
  int minVal;
  int maxVal;

  void validate(int index)
  {
    if (index < minVal || index > maxVal)
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

  int get_val() const { return val; }

  operator int() const { return val; }

  RangedInt &operator=(int v)
  {
    validate(v);
    val = v;
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

