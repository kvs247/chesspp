#include "../src/game.hpp"
#include "../src/piece.hpp"

#include <gtest/gtest.h>
#include <string>
#include <vector>

TEST(PawnTest, Forward)
{
  std::string fen = "rnbqkb1r/ppppp1pp/5p1n/8/8/2N1P3/PPPP1PPP/R1BQKBNR w KQkq - 0 1";
  Game game(fen);
  Pawn pawn(game);

  int index;
  std::vector<int> expected;
  std::vector<int> actual;

  index = 50;
  expected = {};
  actual = pawn.legal_square_indexes(index);
  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());
  ASSERT_EQ(actual, expected);

  index = 51;
  expected = {43, 35};
  actual = pawn.legal_square_indexes(index);
  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());
  ASSERT_EQ(actual, expected);

  index = 44;
  expected = {36};
  actual = pawn.legal_square_indexes(index);
  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());
  ASSERT_EQ(actual, expected);

  index = 15;
  expected = {};
  actual = pawn.legal_square_indexes(index);
  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());
  ASSERT_EQ(actual, expected);

  index = 14;
  expected = {22, 30};
  actual = pawn.legal_square_indexes(index);
  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());
  ASSERT_EQ(actual, expected);

  index = 21;
  expected = {29};
  actual = pawn.legal_square_indexes(index);
  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());
  ASSERT_EQ(actual, expected);
}

TEST(PawnTest, Capture)
{
  std::string fen = "8/8/8/8/8/3p4/4P3/8 w - - 0 1";
  Game game(fen);
  Pawn pawn(game);

  int index;
  std::vector<int> expected;
  std::vector<int> actual;

  index = 52;
  expected = {43, 44, 36};
  actual = pawn.legal_square_indexes(index);
  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());
  ASSERT_EQ(actual, expected);

  index = 43;
  expected = {51, 52};
  actual = pawn.legal_square_indexes(index);
  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());
  ASSERT_EQ(actual, expected);
}

TEST(PawnTest, EnPassant)
{
  std::string fen = "rnbqkbnr/ppp1pppp/8/3pP3/8/7N/PPPP1PPP/RNBQKB1R w KQkq d6 0 4";
  Game game(fen);
  Pawn pawn(game);

  int index = 28;
  std::vector<int> expected = {20, 19};
  std::vector<int> actual = pawn.legal_square_indexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(KnightTest, Center)
{
  std::string fen = "8/8/8/4N3/8/8/8/8 w - - 0 1";
  Game game(fen);
  Knight knight(game);

  int index = 28;
  std::vector<int> expected = {11, 13, 18, 22, 34, 38, 43, 45};
  std::vector<int> actual = knight.legal_square_indexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(KnightTest, Corner)
{
  std::string fen = "8/8/8/8/8/8/8/7N w - - 0 1";
  Game game(fen);
  Knight knight(game);

  int index = 63;
  std::vector<int> expected = {46, 53};
  std::vector<int> actual = knight.legal_square_indexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(KnightTest, Edge)
{
  std::string fen = "8/8/8/8/8/8/8/2N5 w - - 0 1";
  Game game(fen);
  Knight knight(game);

  int index = 58;
  std::vector<int> expected = {41, 43, 48, 52};
  std::vector<int> actual = knight.legal_square_indexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(BishopTest, Center)
{
  std::string fen = "8/8/8/3B4/8/8/8/8 w - - 0 1";
  Game game(fen);
  Bishop bishop(game);

  int index = 27;
  std::vector<int> expected = {0, 9, 18, 20, 13, 6, 34, 41, 48, 36, 45, 54, 63};
  std::vector<int> actual = bishop.legal_square_indexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(BishopTest, CenterBlocked)
{
  std::string fen = "8/5p2/2P5/3B4/4p3/1P6/8/8 w - - 0 1";
  Game game(fen);
  Bishop bishop(game);

  int index = 27;
  std::vector<int> expected = {20, 13, 34, 36};
  std::vector<int> actual = bishop.legal_square_indexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(BishopTest, Corner)
{
  std::string fen = "8/8/8/8/8/8/8/B7 w - - 0 1";
  Game game(fen);
  Bishop bishop(game);

  int index = 56;
  std::vector<int> expected = {49, 42, 35, 28, 21, 14, 7};
  std::vector<int> actual = bishop.legal_square_indexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(BishopTest, Edge)
{
  std::string fen = "8/B7/8/8/8/8/8/8 w - - 0 1";
  Game game(fen);
  Bishop bishop(game);

  int index = 8;
  std::vector<int> expected = {1, 17, 26, 35, 44, 53, 62};
  std::vector<int> actual = bishop.legal_square_indexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(RookTest, Center)
{
  std::string fen = "8/8/8/3R4/8/8/8/8 w - - 0 1";
  Game game(fen);
  Rook rook(game);

  int index = 27;
  std::vector<int> expected = {3, 11, 19, 24, 25, 26, 28, 29, 30, 31, 35, 43, 51, 59};
  std::vector<int> actual = rook.legal_square_indexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(RookTest, CenterBlocked)
{
  std::string fen = "8/3P4/8/2pR1p2/3P4/8/8/8 w - - 0 1";
  Game game(fen);
  Rook rook(game);

  int index = 27;
  std::vector<int> expected = {19, 26, 28, 29};
  std::vector<int> actual = rook.legal_square_indexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(RookTest, Corner)
{
  std::string fen = "7R/8/8/8/8/8/8/8 w - - 0 1";
  Game game(fen);
  Rook rook(game);

  int index = 7;
  std::vector<int> expected = {0, 1, 2, 3, 4, 5, 6, 15, 23, 31, 39, 47, 55, 63};
  std::vector<int> actual = rook.legal_square_indexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(RookTest, Edge)
{
  std::string fen = "8/8/8/7R/8/8/8/8 w - - 0 1";
  Game game(fen);
  Rook rook(game);

  int index = 31;
  std::vector<int> expected = {7, 15, 23, 24, 25, 26, 27, 28, 29, 30, 39, 47, 55, 63};
  std::vector<int> actual = rook.legal_square_indexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(QueenTest, Center)
{
  std::string fen = "8/8/8/3Q4/8/8/8/8 w - - 0 1";
  Game game(fen);
  Queen queen(game);

  int index = 27;
  std::vector<int> expected = {
      0, 9, 18, 36, 45, 54, 63,
      6, 13, 20, 34, 41, 48,
      3, 11, 19, 35, 43, 51, 59,
      24, 25, 26, 28, 29, 30, 31};
  std::vector<int> actual = queen.legal_square_indexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(QueenTest, CenterBlocked)
{
  std::string fen = "8/3P1p2/2p5/P2QP3/8/8/p2P2p1/8 w - - 0 1";
  Game game(fen);
  Queen queen(game);

  int index = 27;
  std::vector<int> expected = {18, 19, 20, 13, 25, 26, 34, 41, 48, 35, 43, 36, 45, 54};
  std::vector<int> actual = queen.legal_square_indexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(QueenTest, Corner)
{
  std::string fen = "8/8/8/8/8/8/8/7Q w - - 0 1";
  Game game(fen);
  Queen queen(game);

  int index = 63;
  std::vector<int> expected = {
      0, 9, 18, 27, 36, 45, 54,
      56, 57, 58, 59, 60, 61, 62,
      7, 15, 23, 31, 39, 47, 55};
  std::vector<int> actual = queen.legal_square_indexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(QueenTest, Edge)
{
  std::string fen = "2Q5/8/8/8/8/8/8/8 w - - 0 1";
  Game game(fen);
  Queen queen(game);

  int index = 2;
  std::vector<int> expected = {
      0, 1, 3, 4, 5, 6, 7, 9, 16,
      10, 18, 26, 34, 42, 50, 58,
      11, 20, 29, 38, 47};
  std::vector<int> actual = queen.legal_square_indexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(KingTest, Center)
{
  std::string fen = "2Q5/8/8/8/8/8/8/8 w - - 0 1";
  Game game(fen);
  King king(game);

  int index = 27;
  std::vector<int> expected = {18, 19, 20, 28, 36, 35, 34, 26};
  std::vector<int> actual = king.legal_square_indexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(KingTest, Corner)
{
  std::string fen = "8/8/8/8/8/8/8/K7 w - - 0 1";
  Game game(fen);
  King king(game);

  int index = 56;
  std::vector<int> expected = {48, 49, 57};
  std::vector<int> actual = king.legal_square_indexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(KingTest, Edge)
{
  std::string fen = "8/8/K7/8/8/8/8/8 w - - 0 1";
  Game game(fen);
  King king(game);

  int index = 16;
  std::vector<int> expected = {8, 9, 17, 25, 24};
  std::vector<int> actual = king.legal_square_indexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}