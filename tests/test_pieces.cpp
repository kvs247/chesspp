#include "../src/game.h"
#include "../src/piece.h"

#include <gtest/gtest.h>
#include <string>
#include <vector>

// Knight
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

// Bishop
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