#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

#include "../src/constants.hpp"
#include "../src/game.hpp"

// Game::State
TEST(GameStateInitialization, FromDefaultFen)
{
  const auto defaultState = Game::State::newGameState();
  std::string defaultFen = startingFenString;
  const auto defaultFenState = Game::State::fromFEN(defaultFen);

  ASSERT_EQ(defaultState, defaultFenState);
}

TEST(GameStateInitialization, FromMidGameFen)
{
  const PiecePlacement piecePlacement = {
      ChessPiece::BlackRook,  ChessPiece::Empty,       ChessPiece::BlackBishop, ChessPiece::Empty,
      ChessPiece::BlackKing,  ChessPiece::BlackBishop, ChessPiece::BlackRook,   ChessPiece::Empty,
      ChessPiece::BlackPawn,  ChessPiece::BlackPawn,   ChessPiece::BlackPawn,   ChessPiece::BlackPawn,
      ChessPiece::BlackQueen, ChessPiece::BlackPawn,   ChessPiece::BlackPawn,   ChessPiece::BlackPawn,
      ChessPiece::Empty,      ChessPiece::Empty,       ChessPiece::BlackKnight, ChessPiece::Empty,
      ChessPiece::Empty,      ChessPiece::BlackKnight, ChessPiece::Empty,       ChessPiece::Empty,
      ChessPiece::Empty,      ChessPiece::Empty,       ChessPiece::Empty,       ChessPiece::Empty,
      ChessPiece::Empty,      ChessPiece::Empty,       ChessPiece::Empty,       ChessPiece::Empty,
      ChessPiece::Empty,      ChessPiece::Empty,       ChessPiece::Empty,       ChessPiece::Empty,
      ChessPiece::BlackPawn,  ChessPiece::WhitePawn,   ChessPiece::Empty,       ChessPiece::Empty,
      ChessPiece::Empty,      ChessPiece::WhitePawn,   ChessPiece::WhiteKnight, ChessPiece::Empty,
      ChessPiece::WhitePawn,  ChessPiece::Empty,       ChessPiece::Empty,       ChessPiece::Empty,
      ChessPiece::WhitePawn,  ChessPiece::WhiteBishop, ChessPiece::WhitePawn,   ChessPiece::WhitePawn,
      ChessPiece::WhiteQueen, ChessPiece::Empty,       ChessPiece::WhitePawn,   ChessPiece::WhitePawn,
      ChessPiece::Empty,      ChessPiece::Empty,       ChessPiece::WhiteKing,   ChessPiece::WhiteRook,
      ChessPiece::Empty,      ChessPiece::WhiteBishop, ChessPiece::WhiteKnight, ChessPiece::WhiteRook};
  const auto activeColor = PieceColor::Black;
  const auto castlingAvailability = CastlingAvailability{false, false, false, true};
  const BoardIndex enPassantIndex = 45;
  const int halfmoveClock = 0;
  const int fullmoveClock = 7;
  const Game::State expected{piecePlacement, activeColor,   castlingAvailability,
                             enPassantIndex, halfmoveClock, fullmoveClock};
  std::string fen = "r1b1kbr1/ppppqppp/2n2n2/8/4pP2/1PN1P3/PBPPQ1PP/2KR1BNR b q f3 0 7";
  const auto actual = Game::State::fromFEN(fen);

  ASSERT_EQ(actual, expected);
}

TEST(GameGetUserMove, ReadMove)
{
  std::istringstream is("e2 e4 a6");
  std::ostringstream os;
  Game defaultGame;
  GameTester game{defaultGame};
  const auto actual = game.testGetUserMove(is, os);

  const std::pair<BoardIndex, BoardIndex> expected{52, 36};

  ASSERT_EQ(actual, expected);
}

// check
TEST(PawnCheck, BlackInCheck)
{
  std::string fen = "8/8/3k4/4P3/8/8/8/8 w - - 0 1";
  Game game(fen);
  PiecePlacement piecePlacement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::Black, piecePlacement));
}

TEST(PawnCheck, WhiteInCheck)
{
  std::string fen = "8/8/8/3p4/4K3/8/8/8 w - - 0 1";
  Game game(fen);
  PiecePlacement piecePlacement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::White, piecePlacement));
}

TEST(KnightCheck, BlackInCheck)
{
  std::string fen = "N7/2k5/8/8/8/8/8/8 w - - 0 1";
  Game game(fen);
  PiecePlacement piece_placement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::Black, piece_placement));
}

TEST(KnightCheck, WhiteInCheck)
{
  std::string fen = "8/8/8/8/5K2/3n4/8/8 w - - 0 1";
  Game game(fen);
  PiecePlacement piece_placement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::White, piece_placement));
}

TEST(BishopCheck, BlackInCheck)
{
  std::string fen = "rnbqkbnr/ppp2ppp/3p4/1B2p3/4P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 1 3";
  Game game(fen);
  PiecePlacement piece_placement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::Black, piece_placement));
}

TEST(BishopCheck, WhiteInCheck)
{
  std::string fen = "rnbqk2r/pppp1ppp/7n/4p3/1b2P3/3P1N2/PPP2PPP/RNBQKB1R w KQkq - 1 4";
  Game game(fen);
  PiecePlacement piece_placement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::White, piece_placement));
}

TEST(RookCheck, BlackInCheck)
{
  std::string fen = "8/3k4/8/8/3R4/8/8/8 w - - 0 1";
  Game game(fen);
  PiecePlacement piece_placement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::Black, piece_placement));
}

TEST(RookCheck, WhiteInCheck)
{
  std::string fen = "8/8/8/8/2K2r2/8/8/8 w - - 0 1";
  Game game(fen);
  PiecePlacement piece_placement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::White, piece_placement));
}

TEST(QueenCheck, BlackInCheckDiag)
{
  std::string fen = "rnbqkbnr/ppppp1pp/5p2/7Q/4P3/8/PPPP1PPP/RNB1KBNR b KQkq - 1 2";
  Game game(fen);
  PiecePlacement piece_placement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::Black, piece_placement));
}

TEST(QueenCheck, WhiteInCheckDiag)
{
  std::string fen = "rnb1kbnr/pppp1ppp/4p3/8/7q/2N2P2/PPPPP1PP/R1BQKBNR w KQkq - 2 3";
  Game game(fen);
  PiecePlacement piece_placement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::White, piece_placement));
}

TEST(QueenCheck, BlackInCheckHorizVert)
{
  std::string fen = "8/2k5/8/8/8/2Q5/8/8 w - - 0 1";
  Game game(fen);
  PiecePlacement piece_placement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::Black, piece_placement));
}

TEST(QueenCheck, WhiteInCheckHorizVert)
{
  std::string fen = "4q3/8/8/8/8/8/4K3/8 w - - 0 1";
  Game game(fen);
  PiecePlacement piece_placement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::White, piece_placement));
}

TEST(KingCheck, BlackInCheck)
{
  std::string fen = "8/8/8/4k3/5K2/8/8/8 w - - 0 1";
  Game game(fen);
  PiecePlacement piece_placement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::Black, piece_placement));
}

TEST(KingCheck, WhiteInCheck)
{
  std::string fen = "8/8/8/4k3/5K2/8/8/8 w - - 0 1";
  Game game(fen);
  PiecePlacement piece_placement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::White, piece_placement));
}