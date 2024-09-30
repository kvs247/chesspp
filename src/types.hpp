#pragma once

#include "logger.hpp"

#include <array>
#include <stdexcept>
#include <initializer_list>
#include <vector>

class BoardIndex
{
private:
  int val;

  void validate(int index)
  {
    if (index < 0 || index > 63)
    {
      throw std::out_of_range("BoardIndex must be in range [0, 63]");
    }
  }

public:
  BoardIndex() = default;

  explicit BoardIndex(int index)
  {
    validate(index);
    val = index;
  }

  int get_val() const
  {
    return val;
  }

  operator int() const
  {
    return val;
  }

  BoardIndex &operator=(int index)
  {
    validate(index);
    val = index;
    return *this;
  }

  static std::vector<BoardIndex> create_vector(std::initializer_list<int> indices)
  {
    std::vector<BoardIndex> result;
    result.reserve(indices.size());
    for (auto index : indices)
    {
      result.emplace_back(index);
    }

    return result;
  }
};

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
