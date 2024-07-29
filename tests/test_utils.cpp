#include <gtest/gtest.h>
#include <array>
#include "../src/utils.h"
#include "../src/constants.h"

TEST(SquareToIndexTest, ValidSquare)
{
  ASSERT_EQ(square_to_index("a8"), 0);
  ASSERT_EQ(square_to_index("h8"), 7);
  ASSERT_EQ(square_to_index("a1"), 56);
  ASSERT_EQ(square_to_index("h1"), 63);
  ASSERT_EQ(square_to_index("d5"), 27);
}

TEST(SquareToIndexTest, InvalidSquare)
{
  ASSERT_THROW(square_to_index(""), std::invalid_argument);
  ASSERT_THROW(square_to_index("a"), std::invalid_argument);
  ASSERT_THROW(square_to_index("a77"), std::invalid_argument);

  ASSERT_THROW(square_to_index("`1"), std::invalid_argument);
  ASSERT_THROW(square_to_index("i2"), std::invalid_argument);
  ASSERT_THROW(square_to_index("@3"), std::invalid_argument);
  ASSERT_THROW(square_to_index("I4"), std::invalid_argument);
  ASSERT_THROW(square_to_index("A0"), std::invalid_argument);
  ASSERT_THROW(square_to_index("H9"), std::invalid_argument);
}

TEST(IndexToSquareTest, ValidIndex)
{
  ASSERT_EQ(index_to_square(0), "a8");
  ASSERT_EQ(index_to_square(7), "h8");
  ASSERT_EQ(index_to_square(56), "a1");
  ASSERT_EQ(index_to_square(63), "h1");
  ASSERT_EQ(index_to_square(27), "d5");
}

TEST(IndexToSquareTest, InvalidIndex)
{
  ASSERT_THROW(index_to_square(-1), std::out_of_range);
  ASSERT_THROW(index_to_square(64), std::out_of_range);
}

TEST(PiecePlacementStringToArrayTest, ValidString)
{
  std::array<char, 64> expected = starting_piece_placement;
  std::array<char, 64> actual = piece_placement_string_to_array(starting_piece_placement_string);
  EXPECT_EQ(actual, expected);
}

TEST(PiecePlacementStringToArrayTest, InvalidString)
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

TEST(PiecePlacementArrayToStringTest, ValidArray)
{
  std::string expected = starting_piece_placement_string;
  std::string actual = piece_placement_array_to_string(starting_piece_placement);
  EXPECT_EQ(actual, expected);
}

TEST(PiecePlacementArrayToStringTest, InvalidArray)
{
  std::array<char, 64> test_array = starting_piece_placement;
  test_array[31] = 'a';
  ASSERT_THROW(piece_placement_array_to_string(test_array), std::invalid_argument);
}

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}