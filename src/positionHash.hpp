#pragma once

#include <array>
#include <optional>
#include <stddef.h>
#include <string>

#include "types.hpp"
#include "utils.hpp"

struct Position
{
  PiecePlacement piecePlacement;
  CastlingAvailability castlingAvailability;
  std::optional<BoardIndex> enPassantIndex;

  bool operator==(const Position &other) const
  {
    return (
        piecePlacement == other.piecePlacement && castlingAvailability == other.castlingAvailability &&
        enPassantIndex == other.enPassantIndex);
  }
};

struct PositionHash
{
  size_t operator()(const Position &position) const
  {
    size_t hash = 17;
    for (auto &piece : position.piecePlacement)
    {
      int x = chessPieceToChar(piece);
      auto val = (x != '\0') ? x : 7;
      hash = hash * 31 + val;
    }

    for (char c : castlingAvailabilityToString(position.castlingAvailability))
    {
      hash = hash * 31 + c;
    }

    if (position.enPassantIndex.has_value())
    {
      int x = position.enPassantIndex.value();
      hash = hash * 31 + x;
    }

    return hash;
  }
};