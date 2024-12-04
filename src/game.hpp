#pragma once

#include <algorithm>
#include <array>
#include <optional>
#include <random>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "config.hpp"
#include "constants.hpp"
#include "moveInput.hpp"
#include "piece.hpp"
#include "positionHash.hpp"
#include "renderer/renderer.hpp"
#include "timeControl.hpp"
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
  friend class ChessTimer;
  friend class MoveInput;
  friend class FrameBuilder;

public:
  struct GameState
  {
    PiecePlacement piecePlacement = startingPiecePlacement;
    PieceColor activeColor = PieceColor::White;
    CastlingAvailability castlingAvailability = startingCastlingAvailability;
    std::optional<BoardIndex> enPassantIndex = std::nullopt;
    int halfmoveClock = 0;
    int fullmoveClock = 1;

    static GameState newGameState() { return {}; };
    static GameState fromFEN(const std::string &fen);

    bool operator==(const GameState &other) const
    {
      return (
          piecePlacement == other.piecePlacement && activeColor == other.activeColor &&
          castlingAvailability == other.castlingAvailability && enPassantIndex == other.enPassantIndex &&
          halfmoveClock == other.halfmoveClock && fullmoveClock == other.fullmoveClock);
    };
  };

  enum class ModalState
  {
    NONE,
    HELP,
    PAWN_PROMOTION,
  };

  Game();
  Game(const std::string &fen);
  Game(const GameState &state);

  std::string getFenStr() const;
  PiecePlacement getPiecePlacement() const { return state.piecePlacement; }
  CastlingAvailability getCastlingAvailability() const { return state.castlingAvailability; }
  std::optional<BoardIndex> getEnPassantIndex() const { return state.enPassantIndex; }
  int getHalfMoveClock() { return state.halfmoveClock; }

  bool isWhiteMove() const;
  bool processNextMove();
  std::vector<BoardIndex> getPieceLegalMoves(const BoardIndex) const;
  bool validateMove(const BoardIndex, const BoardIndex) const;
  static bool isKingInCheck(const PieceColor, const PiecePlacement &);

  bool isGameOver = false;
  std::vector<MoveListItem> moveList;
  std::string message;
  std::unordered_map<Position, int, PositionHash> positionCount;

  ChessTimer timer{*this};

  // make private
  TimeControl whiteTime{config.timeControl};
  TimeControl blackTime{config.timeControl};

  std::string userInput;

  Renderer renderer;

private:
  GameState state;

  ModalState modalState;

  Pawn pawn;
  Knight knight;
  Bishop bishop;
  Rook rook;
  Queen queen;
  King king;

  MoveInput moveInput = MoveInput{*this};

  std::mt19937 randomGenerator;

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

  bool
  testIsSquareUnderAttack(const BoardIndex index, const PieceColor defenderColor, const PiecePlacement &piecePlacement)
  {
    return game.isSquareUnderAttack(index, defenderColor, piecePlacement);
  }

  bool testHandleGameOver() { return game.handleGameOver(); }

  void testIncrementPositionCount() { return game.incrementPositionCount(); }

private:
  Game &game;
};