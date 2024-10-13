#include <gtest/gtest.h>

#include <string>

#include "../src/game.hpp"

TEST(PawnCheck, BlackInCheck) {
  std::string fen = "8/8/3k4/4P3/8/8/8/8 w - - 0 1";
  Game game(fen);
  PiecePlacement piecePlacement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::Black, piecePlacement));
}

TEST(PawnCheck, WhiteInCheck) {
  std::string fen = "8/8/8/3p4/4K3/8/8/8 w - - 0 1";
  Game game(fen);
  PiecePlacement piecePlacement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::White, piecePlacement));
}

TEST(KnightCheck, BlackInCheck) {
  std::string fen = "N7/2k5/8/8/8/8/8/8 w - - 0 1";
  Game game(fen);
  PiecePlacement piece_placement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::Black, piece_placement));
}

TEST(KnightCheck, WhiteInCheck) {
  std::string fen = "8/8/8/8/5K2/3n4/8/8 w - - 0 1";
  Game game(fen);
  PiecePlacement piece_placement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::White, piece_placement));
}

TEST(BishopCheck, BlackInCheck) {
  std::string fen =
      "rnbqkbnr/ppp2ppp/3p4/1B2p3/4P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 1 3";
  Game game(fen);
  PiecePlacement piece_placement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::Black, piece_placement));
}

TEST(BishopCheck, WhiteInCheck) {
  std::string fen =
      "rnbqk2r/pppp1ppp/7n/4p3/1b2P3/3P1N2/PPP2PPP/RNBQKB1R w KQkq - 1 4";
  Game game(fen);
  PiecePlacement piece_placement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::White, piece_placement));
}

TEST(RookCheck, BlackInCheck) {
  std::string fen = "8/3k4/8/8/3R4/8/8/8 w - - 0 1";
  Game game(fen);
  PiecePlacement piece_placement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::Black, piece_placement));
}

TEST(RookCheck, WhiteInCheck) {
  std::string fen = "8/8/8/8/2K2r2/8/8/8 w - - 0 1";
  Game game(fen);
  PiecePlacement piece_placement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::White, piece_placement));
}

TEST(QueenCheck, BlackInCheckDiag) {
  std::string fen =
      "rnbqkbnr/ppppp1pp/5p2/7Q/4P3/8/PPPP1PPP/RNB1KBNR b KQkq - 1 2";
  Game game(fen);
  PiecePlacement piece_placement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::Black, piece_placement));
}

TEST(QueenCheck, WhiteInCheckDiag) {
  std::string fen =
      "rnb1kbnr/pppp1ppp/4p3/8/7q/2N2P2/PPPPP1PP/R1BQKBNR w KQkq - 2 3";
  Game game(fen);
  PiecePlacement piece_placement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::White, piece_placement));
}

TEST(QueenCheck, BlackInCheckHorizVert) {
  std::string fen = "8/2k5/8/8/8/2Q5/8/8 w - - 0 1";
  Game game(fen);
  PiecePlacement piece_placement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::Black, piece_placement));
}

TEST(QueenCheck, WhiteInCheckHorizVert) {
  std::string fen = "4q3/8/8/8/8/8/4K3/8 w - - 0 1";
  Game game(fen);
  PiecePlacement piece_placement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::White, piece_placement));
}

TEST(KingCheck, BlackInCheck) {
  std::string fen = "8/8/8/4k3/5K2/8/8/8 w - - 0 1";
  Game game(fen);
  PiecePlacement piece_placement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::Black, piece_placement));
}

TEST(KingCheck, WhiteInCheck) {
  std::string fen = "8/8/8/4k3/5K2/8/8/8 w - - 0 1";
  Game game(fen);
  PiecePlacement piece_placement = game.getPiecePlacement();
  ASSERT_TRUE(Game::isKingInCheck(PieceColor::White, piece_placement));
}