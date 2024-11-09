#pragma once

#include <istream>
#include <utility>

#include "constants.hpp"
#include "piece.hpp"
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
    static State fromFEN(std::string &fen);

    bool operator==(const State &other) const
    {
      return (piecePlacement == other.piecePlacement && activeColor == other.activeColor &&
              castlingAvailability == other.castlingAvailability && enPassantIndex == other.enPassantIndex &&
              halfmoveClock == other.halfmoveClock && fullmoveClock == other.fullmoveClock);
    };
  };

  Game();
  Game(const State &state);
  Game(std::string &fen) : Game(State::fromFEN(fen)) {};

  PiecePlacement getPiecePlacement() const { return piecePlacement; };
  std::string getFenStr() const;

  bool processNextMove();

  // decided if public or private
  std::vector<BoardIndex> getPieceLegalMoves(const ChessPiece &, const BoardIndex) const;
  bool handleEnPassant(const ChessPiece, const PieceColor, const BoardIndex, const BoardIndex);
  ChessPiece handlePawnPromotion(const ChessPiece, const BoardIndex);
  std::string handleCastling(const BoardIndex, const BoardIndex, const ChessPiece);
  bool validateMove(const BoardIndex, const BoardIndex) const;
  std::vector<BoardIndex> getSamePieceIndexes(const BoardIndex, const BoardIndex) const;
  static bool isKingInCheck(const PieceColor, const PiecePlacement &);
  static bool isSquareUnderAttack(const BoardIndex, const PieceColor, const PiecePlacement &);

  bool isGameOver = false;
  std::vector<MoveListItem> moveList;
  std::string message;

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

  friend struct GameTester;
};

struct GameTester
{
public:
  GameTester(Game &g) : game(g) {}

  std::pair<BoardIndex, BoardIndex> testGetUserMove(std::istream &is, std::ostream &os)
  {
    return game.getUserMove(is, os);
  };

private:
  Game &game;
};