#include "../src/constants.h"
#include "../src/utils.h"

#include <array>
#include <gtest/gtest.h>

TEST(AlgebraicToIndexTest, ValidInput)
{
  ASSERT_EQ(algebraic_to_index("a8"), 0);
  ASSERT_EQ(algebraic_to_index("h8"), 7);
  ASSERT_EQ(algebraic_to_index("a1"), 56);
  ASSERT_EQ(algebraic_to_index("h1"), 63);
  ASSERT_EQ(algebraic_to_index("d5"), 27);
}

TEST(AlgebraicToIndexTest, InvalidInput)
{
  ASSERT_THROW(algebraic_to_index(""), std::invalid_argument);
  ASSERT_THROW(algebraic_to_index("a"), std::invalid_argument);
  ASSERT_THROW(algebraic_to_index("a77"), std::invalid_argument);

  ASSERT_THROW(algebraic_to_index("`1"), std::invalid_argument);
  ASSERT_THROW(algebraic_to_index("i2"), std::invalid_argument);
  ASSERT_THROW(algebraic_to_index("@3"), std::invalid_argument);
  ASSERT_THROW(algebraic_to_index("I4"), std::invalid_argument);
  ASSERT_THROW(algebraic_to_index("A0"), std::invalid_argument);
  ASSERT_THROW(algebraic_to_index("H9"), std::invalid_argument);
}

TEST(IndexToAlgebraicTest, ValidInput)
{
  ASSERT_EQ(index_to_algebraic(0), "a8");
  ASSERT_EQ(index_to_algebraic(7), "h8");
  ASSERT_EQ(index_to_algebraic(56), "a1");
  ASSERT_EQ(index_to_algebraic(63), "h1");
  ASSERT_EQ(index_to_algebraic(27), "d5");
}

TEST(IndexToAlgebraicTest, InvalidInput)
{
  ASSERT_THROW(index_to_algebraic(-1), std::out_of_range);
  ASSERT_THROW(index_to_algebraic(64), std::out_of_range);
}

TEST(PiecePlacementStringToArrayTest, ValidInput)
{
  std::array<char, 64> expected = starting_piece_placement;
  std::array<char, 64> actual = piece_placement_string_to_array(starting_piece_placement_string);
  ASSERT_EQ(actual, expected);
}

TEST(PiecePlacementStringToArrayTest, InvalidInput)
{
  std::string test_string = starting_piece_placement_string + "K";
  ASSERT_THROW(
      piece_placement_string_to_array(test_string),
      std::invalid_argument);
  ASSERT_THROW(
      piece_placement_string_to_array("rnbkqbnr"),
      std::invalid_argument);

  ASSERT_THROW(
      piece_placement_string_to_array("rnbqkbnr/appppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"),
      std::invalid_argument);
}

TEST(PiecePlacementArrayToStringTest, ValidInput)
{
  std::string expected = starting_piece_placement_string;
  std::string actual = piece_placement_array_to_string(starting_piece_placement);
  ASSERT_EQ(actual, expected);
}

TEST(PiecePlacementArrayToStringTest, InvalidInput)
{
  std::array<char, 64> test_array = starting_piece_placement;
  test_array[31] = 'a';
  ASSERT_THROW(piece_placement_array_to_string(test_array), std::invalid_argument);
}

TEST(IndexToFileRank, ValidInput)
{
  FileRank expected;

  expected = {1, 8};
  ASSERT_EQ(index_to_file_rank(0), expected);
  expected = {8, 8};
  ASSERT_EQ(index_to_file_rank(7), expected);
  expected = {1, 1};
  ASSERT_EQ(index_to_file_rank(56), expected);
  expected = {8, 1};
  ASSERT_EQ(index_to_file_rank(63), expected);
  expected = {3, 6};
  ASSERT_EQ(index_to_file_rank(18), expected);
}

TEST(IndexToFileRank, InvalidInput)
{
  ASSERT_THROW(index_to_file_rank(-1), std::out_of_range);
  ASSERT_THROW(index_to_file_rank(64), std::out_of_range);
}

TEST(FileRankToIndex, ValidInput)
{
  ASSERT_EQ(file_rank_to_index({1, 1}), 56);
  ASSERT_EQ(file_rank_to_index({1, 8}), 0);
  ASSERT_EQ(file_rank_to_index({8, 1}), 63);
  ASSERT_EQ(file_rank_to_index({8, 8}), 7);
  ASSERT_EQ(file_rank_to_index({3, 6}), 18);
}

TEST(FileRankToIndex, InvalidInput)
{
  ASSERT_THROW(file_rank_to_index({0, 1}), std::out_of_range);
  ASSERT_THROW(file_rank_to_index({1, 0}), std::out_of_range);
  ASSERT_THROW(file_rank_to_index({9, 1}), std::out_of_range);
  ASSERT_THROW(file_rank_to_index({1, 9}), std::out_of_range);
}
