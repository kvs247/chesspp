#pragma once

#include <istream>
#include <unordered_map>
#include <utility>

#include "constants.hpp"
#include "piece.hpp"
#include "positionHash.hpp"
#include "types.hpp"

class Game
{
  friend class Piece;
  friend class Pawn;
  friend class Knight;
  friend class Bishop;
  friend class Rook;
  friend class Queen;
  friend class King;

public:
  struct State
  {
    PiecePlacement piecePlacement = startingPiecePlacement;
    PieceColor activeColor = PieceColor::White;
    CastlingAvailability castlingAvailability = startingCastlingAvailability;
    std::optional<BoardIndex> enPassantIndex = std::nullopt;
    int halfmoveClock = 0;
    int fullmoveClock = 1;

    static State newGameState() { return {}; };
    static State fromFEN(const std::string &fen);

    bool operator==(const State &other) const
    {
      return (piecePlacement == other.piecePlacement && activeColor == other.activeColor &&
              castlingAvailability == other.castlingAvailability && enPassantIndex == other.enPassantIndex &&
              halfmoveClock == other.halfmoveClock && fullmoveClock == other.fullmoveClock);
    };
  };

  Game();
  Game(const State &state);
  Game(const std::string &fen);

  std::string getFenStr() const;
  PiecePlacement getPiecePlacement() const { return piecePlacement; }
  CastlingAvailability getCastlingAvailability() const { return castlingAvailability; }
  std::optional<BoardIndex> getEnPassantIndex() const { return enPassantIndex; }
  int getHalfMoveClock() { return halfmoveClock; }

  bool isWhiteMove() const;
  bool processNextMove();
  std::vector<BoardIndex> getPieceLegalMoves(const BoardIndex) const;
  bool validateMove(const BoardIndex, const BoardIndex) const;
  static bool isKingInCheck(const PieceColor, const PiecePlacement &);

  bool isGameOver = false;
  std::vector<MoveListItem> moveList;
  std::string message;
  std::unordered_map<Position, int, PositionHash> positionCount;

private:
  PiecePlacement piecePlacement;
  PieceColor activeColor;
  CastlingAvailability castlingAvailability;
  std::optional<BoardIndex> enPassantIndex;
  int halfmoveClock;
  int fullmoveClock;

  Pawn pawn;
  Knight knight;
  Bishop bishop;
  Rook rook;
  Queen queen;
  King king;

  std::pair<BoardIndex, BoardIndex> getNextMove();
  std::pair<BoardIndex, BoardIndex> getUserMove(std::istream &, std::ostream &);
  std::pair<BoardIndex, BoardIndex> generateCpuMove(const PieceColor);
  void updateHalfMoveClock(const ChessPiece, const ChessPiece);
  bool handleEnPassant(const BoardIndex, const BoardIndex);
  ChessPiece handlePawnPromotion(const ChessPiece, const BoardIndex);
  std::string handleCastling(const BoardIndex, const BoardIndex);
  bool handleGameOver();
  void incrementPositionCount();
  std::vector<BoardIndex> getSamePieceIndexes(const BoardIndex, const BoardIndex) const;
  static bool isSquareUnderAttack(const BoardIndex, const PieceColor, const PiecePlacement &);

  friend struct GameTester;
};

struct GameTester
{
public:
  GameTester(Game &g) : game(g) {}

  PiecePlacement testGetPiecePlacement() const { return game.getPiecePlacement(); }

  std::pair<BoardIndex, BoardIndex> testGetUserMove(std::istream &is, std::ostream &os)
  {
    return game.getUserMove(is, os);
  };

  std::vector<BoardIndex> testGetPieceLegalMoves(const BoardIndex index) const
  {
    return game.getPieceLegalMoves(index);
  };

  void testUpdateHalfMoveClock(const ChessPiece fromPiece, const ChessPiece toPiece)
  {
    return game.updateHalfMoveClock(fromPiece, toPiece);
  }

  bool testHandleEnPassant(const BoardIndex fromIndex, const BoardIndex toIndex)
  {
    return game.handleEnPassant(fromIndex, toIndex);
  }

  ChessPiece testHandlePawnPromotion(const ChessPiece fromPiece, const BoardIndex toIndex)
  {
    return game.handlePawnPromotion(fromPiece, toIndex);
  }

  std::string testHandleCastling(const BoardIndex fromIndex, const BoardIndex toIndex)
  {
    return game.handleCastling(fromIndex, toIndex);
  }

  std::vector<BoardIndex> testGetSamePieceIndexes(const BoardIndex fromIndex, const BoardIndex toIndex) const
  {
    return game.getSamePieceIndexes(fromIndex, toIndex);
  }

  bool testIsSquareUnderAttack(const BoardIndex index, const PieceColor defenderColor,
                               const PiecePlacement &piecePlacement)
  {
    return game.isSquareUnderAttack(index, defenderColor, piecePlacement);
  }

  bool testHandleGameOver() { return game.handleGameOver(); }

  void testIncrementPositionCount() { return game.incrementPositionCount(); }

private:
  Game &game;
};