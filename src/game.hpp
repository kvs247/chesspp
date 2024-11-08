#pragma once

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
    PiecePlacement piecePlacement;
    PieceColor activeColor;
    CastlingAvailability castlingAvailability;
    std::optional<BoardIndex> enPassantIndex;
    int halfmoveClock;
    int fullmoveClock;

    static State newGameState() { return {}; };
  };

  Game();
  Game(State);
  Game(std::string &);

  bool move();
  std::string getFenStr() const;
  PiecePlacement getPiecePlacement() const;
  void readMove(BoardIndex &, BoardIndex &) const;
  std::vector<BoardIndex> getPieceLegalMoves(const ChessPiece &, const BoardIndex) const;
  bool handleEnPassant(const ChessPiece, const PieceColor, const BoardIndex, const BoardIndex);
  ChessPiece handlePawnPromotion(const ChessPiece, const BoardIndex);
  std::string handleCastling(const BoardIndex, const BoardIndex, const ChessPiece);
  std::pair<BoardIndex, BoardIndex> generateCpuMove(const PieceColor);
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
};