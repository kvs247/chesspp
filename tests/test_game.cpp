#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

#include "../src/config.hpp"
#include "../src/constants.hpp"
#include "../src/game.hpp"

// public methods

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

TEST(GameGetPieceLegalMoves, InvokesCorrectPieceClass)
{
  std::string fen = "rnbqkb1r/ppp2ppp/5n2/3pp3/P4P2/2P5/1P1PP1PP/RNBQKBNR w KQkq d6 0 4";
  Game startingGame(fen);
  GameTester game{startingGame};

  Pawn pawn(startingGame);
  BoardIndex pawnIndex = 28;
  auto pawnActual = game.testGetPieceLegalMoves(pawnIndex);
  auto pawnExpected = pawn.legalSquareIndexes(pawnIndex);
  ASSERT_EQ(pawnActual, pawnExpected);

  Knight knight(startingGame);
  BoardIndex knightIndex = 21;
  auto knightActual = game.testGetPieceLegalMoves(knightIndex);
  auto knightExpected = knight.legalSquareIndexes(knightIndex);
  ASSERT_EQ(knightActual, knightExpected);

  Bishop bishop(startingGame);
  BoardIndex bishopIndex = 2;
  auto bishopActual = game.testGetPieceLegalMoves(bishopIndex);
  auto bishopExpected = bishop.legalSquareIndexes(bishopIndex);
  ASSERT_EQ(bishopActual, bishopExpected);

  Rook rook(startingGame);
  BoardIndex rookIndex = 56;
  auto rookActual = game.testGetPieceLegalMoves(rookIndex);
  auto rookExpected = rook.legalSquareIndexes(rookIndex);
  ASSERT_EQ(rookActual, rookExpected);

  Queen queen(startingGame);
  BoardIndex queenIndex = 59;
  auto queenActual = game.testGetPieceLegalMoves(queenIndex);
  auto queenExpected = queen.legalSquareIndexes(queenIndex);
  ASSERT_EQ(queenActual, queenExpected);

  King king(startingGame);
  BoardIndex kingIndex = 4;
  auto kingActual = game.testGetPieceLegalMoves(kingIndex);
  auto kingExpected = king.legalSquareIndexes(kingIndex);
  ASSERT_EQ(kingActual, kingExpected);
}

TEST(GameGetPieceLegalMoves, ThrowsErrorOnEmptyPiece)
{
  Game game;
  GameTester gameTester{game};

  ASSERT_THROW(gameTester.testGetPieceLegalMoves(30), std::invalid_argument);
}

TEST(GameValidateMove, NoPieceSelected)
{
  Game game;

  ASSERT_FALSE(game.validateMove(35, 27));
}

TEST(GameValidateMove, ValidMoveWrongTurnOrder)
{
  Game game;
  const auto originalConfig = config;
  config.disableTurnOrder = false;

  ASSERT_FALSE(game.validateMove(8, 16));

  config = originalConfig;
}

TEST(GameValidateMove, InvalidMove)
{
  Game game("rnbqkbnr/pppp1ppp/8/4p3/P3P3/8/1PPP1PPP/RNBQKBNR b KQkq a3 0 2");

  ASSERT_FALSE(game.validateMove(56, 8));  // white rook
  ASSERT_FALSE(game.validateMove(57, 41)); // white knight
  ASSERT_FALSE(game.validateMove(59, 51)); // white queen
  ASSERT_FALSE(game.validateMove(60, 45)); // white king
  ASSERT_FALSE(game.validateMove(36, 27)); // white pawn
}

TEST(GameValidMove, ValidMove)
{
  Game game("rnbqkbnr/pppp1ppp/8/4p3/P3P3/8/1PPP1PPP/RNBQKBNR b KQkq a3 0 2");

  ASSERT_TRUE(game.validateMove(56, 40)); // white rook
  ASSERT_TRUE(game.validateMove(57, 40)); // white knight
  ASSERT_TRUE(game.validateMove(59, 31)); // white queen
  ASSERT_TRUE(game.validateMove(60, 52)); // white king
  ASSERT_TRUE(game.validateMove(32, 24)); // white pawn
}

// private methods

TEST(GameHandleEnPassant, NullBehavior)
{
  Game game;
  GameTester gameTester{game};

  ASSERT_FALSE(gameTester.testHandleEnPassant(52, 44));
}

TEST(GameHandleEnPassant, CreateEnPassantOpportunity)
{
  Game game;
  GameTester gameTester{game};

  auto isEnPassantCapture = gameTester.testHandleEnPassant(52, 36);

  ASSERT_FALSE(isEnPassantCapture);
  ASSERT_EQ(game.getEnPassantIndex(), 44);
}

TEST(GameHandleEnPassant, EnPassantCapture)
{
  Game game("r1bqkbnr/ppp1pppp/n7/3pP3/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 3");
  GameTester gameTester{game};

  auto isEnPassantCapture = gameTester.testHandleEnPassant(28, 19);

  ASSERT_TRUE(isEnPassantCapture);
  ASSERT_EQ(game.getPiecePlacement()[27], ChessPiece::Empty);
}

TEST(GameHandleEnPassant, ThrowsErrorOnEmptyPiece)
{
  Game game;
  GameTester gameTester(game);

  ASSERT_THROW(gameTester.testHandleEnPassant(30, 38), std::invalid_argument);
}

TEST(GameHandlePawnPromotion, PieceIsNotPawn)
{
  Game game("rnbq1bnr/pppkpPpp/8/8/8/5N2/PPpP1PPP/RNBQKB1R w KQ - 0 6");
  GameTester gameTester(game);

  ASSERT_EQ(gameTester.testHandlePawnPromotion(ChessPiece::WhiteKnight, 40), ChessPiece::Empty);
  ASSERT_EQ(gameTester.testHandlePawnPromotion(ChessPiece::BlackKnight, 21), ChessPiece::Empty);
}

TEST(GameHandlePawnPromotion, PawnIsNotPromotable)
{
  Game game("rnbqkbnr/ppp1pppp/4P3/8/8/3p4/PPPP1PPP/RNBQKBNR w KQkq - 0 4");
  GameTester gameTester(game);

  ASSERT_EQ(gameTester.testHandlePawnPromotion(ChessPiece::WhitePawn, 20), ChessPiece::Empty);
  ASSERT_EQ(gameTester.testHandlePawnPromotion(ChessPiece::BlackPawn, 43), ChessPiece::Empty);
  ASSERT_EQ(gameTester.testHandlePawnPromotion(ChessPiece::BlackPawn, 16), ChessPiece::Empty);
}

TEST(GameHandlePawnPromotion, WhitePawnIsPromoted)
{
  Game game("rnbq1bnr/pppkpPpp/8/8/8/5N2/PPpP1PPP/RNBQKB1R w KQ - 0 6");
  GameTester gameTester(game);

  const auto promotedPiece = gameTester.testHandlePawnPromotion(ChessPiece::WhitePawn, 6);

  ASSERT_EQ(promotedPiece, ChessPiece::WhiteQueen);
  ASSERT_EQ(game.getPiecePlacement()[6], ChessPiece::WhiteQueen);
}

TEST(GameHandlePawnPromotion, BlackPawnIsPromoted)
{
  Game game("rnbq1bQr/pppkp1pp/8/8/8/5N2/PPpP1PPP/RNBQKB1R b KQ - 0 6");
  GameTester gameTester(game);

  const auto promotedPiece = gameTester.testHandlePawnPromotion(ChessPiece::BlackPawn, 59);

  ASSERT_EQ(promotedPiece, ChessPiece::BlackQueen);
  ASSERT_EQ(game.getPiecePlacement()[59], ChessPiece::BlackQueen);
}

TEST(GameHandleCastling, PieceIsNotKing)
{
  Game game("rn2kbnr/ppp1pppp/3qb3/3p4/8/3BPN2/PPPP1PPP/RNBQK2R w KQkq - 4 4");
  GameTester gameTester(game);

  const auto castlingResult = gameTester.testHandleCastling(43, 25);

  ASSERT_TRUE(castlingResult.empty());
}

TEST(GameHandleCastling, WhiteKingIsNotCastling)
{
  Game game("rn2kbnr/ppp1pppp/3qb3/3p4/8/3BPN2/PPPP1PPP/RNBQK2R w KQkq - 4 4");
  GameTester gameTester(game);

  const auto castlingResult = gameTester.testHandleCastling(60, 61);

  ASSERT_TRUE(castlingResult.empty());
}

TEST(GameHandleCastling, BlackKingIsNotCastling)
{
  Game game("r3kbnr/ppp1pppp/2nqb3/3p4/8/3BPN2/PPPPQPPP/RNB2K1R b kq - 7 5");
  GameTester gameTester(game);

  const auto castlingResult = gameTester.testHandleCastling(4, 3);

  ASSERT_TRUE(castlingResult.empty());
}

TEST(GameHandleCastling, WhiteKingCastle)
{
  Game game("rn2kbnr/ppp1pppp/3qb3/3p4/8/3BPN2/PPPP1PPP/RNBQK2R w KQkq - 4 4");
  GameTester gameTester(game);

  const auto castlingResult = gameTester.testHandleCastling(60, 62);

  ASSERT_EQ(castlingResult, shortCastleString);
  ASSERT_EQ(game.getPiecePlacement()[61], ChessPiece::WhiteRook);
}

TEST(GameHandleCastling, BlackKingCastle)
{
  Game game("r3kbnr/ppp1pppp/2nqb3/3p4/8/3BPN2/PPPPQPPP/RNB2K1R b kq - 7 5");
  GameTester gameTester(game);

  const auto castlingResult = gameTester.testHandleCastling(4, 2);

  ASSERT_EQ(castlingResult, longCastleString);
  ASSERT_EQ(game.getPiecePlacement()[3], ChessPiece::BlackRook);
}

TEST(GameHandleCastling, ThrowsErrorOnEmptyPiece)
{
  Game game("r3kbnr/ppp1pppp/2nqb3/3p4/8/3BPN2/PPPPQPPP/RNB2K1R b kq - 7 5");
  GameTester gameTester(game);

  ASSERT_THROW(gameTester.testHandleCastling(3, 2), std::invalid_argument);
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