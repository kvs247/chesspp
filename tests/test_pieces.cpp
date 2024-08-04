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