#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "../src/game.hpp"
#include "../src/piece.hpp"

TEST(PawnTest, WhiteForward)
{
  std::string fen = "rnbqkb1r/ppppp1pp/5p1n/8/8/2N1P3/PPPP1PPP/R1BQKBNR w KQkq - 0 1";
  Game game(fen);
  Pawn pawn(game);

  BoardIndex index;
  std::vector<BoardIndex> expected;
  std::vector<BoardIndex> actual;

  index = 50;
  expected = {};
  actual = pawn.legalSquareIndexes(index);
  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());
  ASSERT_EQ(actual, expected);

  index = 51;
  expected = BoardIndex::create_vector({43, 35});
  actual = pawn.legalSquareIndexes(index);
  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());
  ASSERT_EQ(actual, expected);

  index = 44;
  expected = BoardIndex::create_vector({36});
  actual = pawn.legalSquareIndexes(index);
  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());
  ASSERT_EQ(actual, expected);
};

TEST(PawnTest, BlackForward)
{
  std::string fen = "rnbqkb1r/ppppp1pp/5p1n/8/8/2N1P3/PPPP1PPP/R1BQKBNR w KQkq - 0 1";
  Game game(fen);
  Pawn pawn(game);

  BoardIndex index;
  std::vector<BoardIndex> expected;
  std::vector<BoardIndex> actual;

  index = 15;
  expected = {};
  actual = pawn.legalSquareIndexes(index);
  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());
  ASSERT_EQ(actual, expected);

  index = 14;
  expected = BoardIndex::create_vector({22, 30});
  actual = pawn.legalSquareIndexes(index);
  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());
  ASSERT_EQ(actual, expected);

  index = 21;
  expected = BoardIndex::create_vector({29});
  actual = pawn.legalSquareIndexes(index);
  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());
  ASSERT_EQ(actual, expected);
}

TEST(PawnTest, WhiteCapture)
{
  std::string fen = "r1bqkbnr/pp2pppp/8/3pNP2/2pnP3/8/PPPP2PP/RNBQKB1R w KQkq - 0 1";
  Game game(fen);
  Pawn pawn(game);

  BoardIndex index;
  std::vector<BoardIndex> expected;
  std::vector<BoardIndex> actual;

  index = 36;
  expected = BoardIndex::create_vector({27});
  actual = pawn.legalSquareIndexes(index);
  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());
  ASSERT_EQ(actual, expected);
}

TEST(PawnTest, BlackCapture)
{
  std::string fen = "rnbqkbnr/ppp1pppp/8/3p4/4N3/8/PPPPPPPP/RNBQKB1R w KQkq - 0 1";
  Game game(fen);
  Pawn pawn(game);

  BoardIndex index;
  std::vector<BoardIndex> expected;
  std::vector<BoardIndex> actual;

  index = 27;
  expected = BoardIndex::create_vector({35, 36});
  actual = pawn.legalSquareIndexes(index);
  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());
  ASSERT_EQ(actual, expected);
}

TEST(PawnTest, WhiteEnPassant)
{
  std::string fen = "rnbqkbnr/ppp1pppp/8/3pP3/8/7N/PPPP1PPP/RNBQKB1R w KQkq d6 0 4";
  Game game(fen);
  Pawn pawn(game);

  BoardIndex index(28);
  auto expected = BoardIndex::create_vector({20, 19});
  auto actual = pawn.legalSquareIndexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(PawnTest, WhiteCreateSelfCheck)
{
  std::string fen = "rnb1kbnr/pppp1ppp/4p3/8/7q/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  Game game(fen);
  Pawn pawn(game);

  BoardIndex index(53);
  auto expected = BoardIndex::create_vector({});
  auto actual = pawn.legalSquareIndexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(PawnTest, BlackCreateSelfCheck)
{
  std::string fen = "rnbq1bnr/pppp1ppp/8/8/3kp2R/8/PPPPPPPP/RNBQKBN1 w Q - 0 1";
  Game game(fen);
  Pawn pawn(game);

  BoardIndex index(36);
  auto expected = BoardIndex::create_vector({});
  auto actual = pawn.legalSquareIndexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(PawnTest, BlackEnPassant)
{
  std::string fen = "rnbqkbnr/1ppppppp/8/8/pP5P/7R/P1PPPPP1/RNBQKBN1 b Qkq b3 0 3";
  Game game(fen);
  Pawn pawn(game);

  BoardIndex index(32);
  auto expected = BoardIndex::create_vector({40, 41});
  auto actual = pawn.legalSquareIndexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(KnightTest, WhiteCenter)
{
  std::string fen = "rnbqkbnr/ppp1p1pp/3p1p2/8/4N3/8/PPPPPPPP/RNBQKB1R w KQkq - 0";
  Game game(fen);
  Knight knight(game);

  BoardIndex index(36);
  auto expected = BoardIndex::create_vector({19, 21, 26, 30, 42, 46});
  auto actual = knight.legalSquareIndexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(KnightTest, BlackCenter)
{
  std::string fen = "rnbqkb1r/pppppppp/1n6/8/2P5/8/PP1PPPPP/RNBQKBNR w KQkq - 0 1";
  Game game(fen);
  Knight knight(game);

  BoardIndex index(17);
  auto expected = BoardIndex::create_vector({27, 32, 34});
  auto actual = knight.legalSquareIndexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(KnightTest, WhiteCreateSelfCheck)
{
  std::string fen = "rnb1kbnr/pppp1ppp/4p3/8/7q/5P2/PPPPPNPP/R1BQKBNR w KQkq - 0 1";
  Game game(fen);
  Knight knight(game);

  BoardIndex index(53);
  auto expected = BoardIndex::create_vector({});
  auto actual = knight.legalSquareIndexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(KnightTest, BlackCreateSelfCheck)
{
  std::string fen = "rnbqkb1r/ppppnppp/8/8/7P/4R3/PPPPPPP1/RNBQKBN1 w Qkq - 0 1";
  Game game(fen);
  Knight knight(game);

  BoardIndex index(12);
  auto expected = BoardIndex::create_vector({});
  auto actual = knight.legalSquareIndexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(BishopTest, WhiteCenter)
{
  std::string fen = "rnbqkbnr/pppppp1p/6p1/8/3B4/1P6/P1PPPPPP/RNBQK1NR w KQkq - 0 1";
  Game game(fen);
  Bishop bishop(game);

  BoardIndex index(35);
  auto expected = BoardIndex::create_vector({8, 17, 26, 42, 49, 44, 28, 21, 14, 7});
  std::vector<BoardIndex> actual = bishop.legalSquareIndexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(BishopTest, BlackCenter)
{
  std::string fen = "rnbqk1nr/pppppppp/7b/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  Game game(fen);
  Bishop bishop(game);

  BoardIndex index(23);
  auto expected = BoardIndex::create_vector({30, 37, 44, 51});
  std::vector<BoardIndex> actual = bishop.legalSquareIndexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(BishopTest, WhiteCreateSelfCheck)
{
  std::string fen = "rnbqk1nr/pppp1ppp/4p3/8/1b6/3P4/PPPBPPPP/RN1QKBNR w KQkq - 0 1";
  Game game(fen);
  Bishop bishop(game);

  BoardIndex index(51);
  auto expected = BoardIndex::create_vector({42, 33});
  std::vector<BoardIndex> actual = bishop.legalSquareIndexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(BishopTest, BlackCreateSelfCheck)
{
  std::string fen = "rnbqk1nr/ppppbppp/8/8/8/4R3/PPPPPPPP/RNBQKBN1 w Qkq - 0 1";
  Game game(fen);
  Bishop bishop(game);

  BoardIndex index(12);
  auto expected = BoardIndex::create_vector({});
  std::vector<BoardIndex> actual = bishop.legalSquareIndexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(RookTest, WhiteCenter)
{
  std::string fen = "rnbqkbnr/pppppppp/8/8/2R5/8/PPPPPPPP/RNBQKBN1 w Qkq - 0 1";
  Game game(fen);
  Rook rook(game);

  BoardIndex index(34);
  auto expected = BoardIndex::create_vector({32, 33, 35, 36, 37, 38, 39, 42, 26, 18, 10});
  auto actual = rook.legalSquareIndexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(RookTest, BlackCenter)
{
  std::string fen = "rnbqkbn1/pppppppp/8/8/8/7r/PPPPPPPP/RNBQKBNR w KQq - 0 1";
  Game game(fen);
  Rook rook(game);

  BoardIndex index(47);
  auto expected = BoardIndex::create_vector({23, 31, 39, 55, 46, 45, 44, 43, 42, 41, 40});
  auto actual = rook.legalSquareIndexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(RookTest, WhiteCreateSelfCheck)
{
  std::string fen = "rnb1kbnr/pppppppp/4q3/8/8/5P2/PPPPR1PP/RNBQKBN1 w Qkq - 0 1";
  Game game(fen);
  Rook rook(game);

  BoardIndex index(52);
  auto expected = BoardIndex::create_vector({44, 36, 28, 20});
  auto actual = rook.legalSquareIndexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(RookTest, BlackCreateSelfCheck)
{
  std::string fen = "rnbq1bn1/pppppppp/k7/1r6/8/3Q4/PPPPPPPP/RNB1KBNR w KQ - 0 1";
  Game game(fen);
  Rook rook(game);

  BoardIndex index(25);
  auto expected = BoardIndex::create_vector({});
  auto actual = rook.legalSquareIndexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(QueenTest, WhiteCenter)
{
  std::string fen = "rnbqkbnr/pppppppp/8/8/2Q5/8/PPPPPPPP/RNB1KBNR w KQkq - 0 1";
  Game game(fen);
  Queen queen(game);

  BoardIndex index(34);
  auto expected = BoardIndex::create_vector({26, 18, 10, 25, 16, 33, 32, 41, 42, 43, 35, 36, 37, 38, 39, 27, 20, 13});
  auto actual = queen.legalSquareIndexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(QueenTest, BlackCenter)
{
  std::string fen = "rnb1kbnr/pppppppp/8/7q/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  Game game(fen);
  Queen queen(game);

  BoardIndex index(31);
  auto expected = BoardIndex::create_vector({23, 22, 30, 29, 28, 27, 26, 25, 24, 38, 45, 52, 39, 47, 55});
  auto actual = queen.legalSquareIndexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(QueenTest, WhiteCreateSelfCheck)
{
  std::string fen = "rnbqk1nr/pppppppp/8/8/7b/8/PPPPPQPP/RNB1KBNR w KQkq - 0 1";
  Game game(fen);
  Queen queen(game);

  BoardIndex index(53);
  auto expected = BoardIndex::create_vector({46, 39});
  auto actual = queen.legalSquareIndexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(QueenTest, BlackCreateSelfCheck)
{
  std::string fen = "rnb1kbnr/pppqpppp/8/1B6/8/8/PPPPPPPP/RNBQK1NR w KQkq - 0 1";
  Game game(fen);
  Queen queen(game);

  BoardIndex index(11);
  auto expected = BoardIndex::create_vector({18, 25});
  auto actual = queen.legalSquareIndexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(KingTest, WhiteCenter)
{
  std::string fen = "rnb1kbnr/ppp1pppp/8/5q2/7p/6K1/PPPPPPPP/RNBQ1BNR w kq - 0 1";
  Game game(fen);
  King king(game);

  BoardIndex index(46);
  auto expected = BoardIndex::create_vector({39});
  auto actual = king.legalSquareIndexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}

TEST(KingTest, BlackCenter)
{
  std::string fen = "rnbq1bnr/pppppppp/8/3k4/4Q3/8/PPPPPPPP/RNB1KBNR w KQ - 0 1";
  Game game(fen);
  King king(game);

  BoardIndex index(27);
  auto expected = BoardIndex::create_vector({36, 19, 26});
  auto actual = king.legalSquareIndexes(index);

  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(actual, expected);
}
