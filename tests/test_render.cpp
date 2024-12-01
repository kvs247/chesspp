#include <gtest/gtest.h>
#include <string>

#include "../src/types.hpp"

// TEST(HandleAmbiguousMove, FourRooks)
// {
//   const BoardIndex fromIndex = 29;
//   const std::vector<BoardIndex> otherIndexes = {44, 12, 25};

//   const std::string expected = "f";
//   const std::string actual = handleAmbiguousMove(fromIndex, otherIndexes);

//   ASSERT_EQ(expected, actual);
// }

// TEST(HandleAmbiguousMove, ThreeQueens)
// {
//   const BoardIndex fromIndex = 43;
//   const std::vector<BoardIndex> otherIndexes = {59, 45};

//   const std::string expected = "d3";
//   const std::string actual = handleAmbiguousMove(fromIndex, otherIndexes);

//   ASSERT_EQ(expected, actual);
// }