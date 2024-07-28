#include <gtest/gtest.h>
#include "../src/utils.h"

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

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}