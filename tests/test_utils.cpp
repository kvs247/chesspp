#include <gtest/gtest.h>

#include <array>

#include "../src/constants.hpp"
#include "../src/types.hpp"
#include "../src/utils.hpp"

TEST(IsChessPieceTest, ValidInput)
{
  ASSERT_TRUE(isChessPiece('q'));
  ASSERT_TRUE(isChessPiece('Q'));
}

TEST(IsChessPieceTest, InvalidInput)
{
  ASSERT_FALSE(isChessPiece('a'));
  ASSERT_FALSE(isChessPiece('%'));
}

TEST(ColorToCharTest, ValidInput)
{
  ASSERT_EQ(colorToChar(PieceColor::Black), 'b');
  ASSERT_EQ(colorToChar(PieceColor::White), 'w');
}

TEST(CharToColorTest, ValidInput)
{
  ASSERT_EQ(charToColor('w'), PieceColor::White);
  ASSERT_EQ(charToColor('b'), PieceColor::Black);
}

TEST(CharToColorTest, InvalidInput)
{
  ASSERT_THROW(charToColor('k'), std::invalid_argument);
  ASSERT_THROW(charToColor('&'), std::invalid_argument);
}

TEST(ChessPieceToCharTest, ValidInput)
{
  ASSERT_EQ(chessPieceToChar(ChessPiece::BlackRook), 'r');
  ASSERT_EQ(chessPieceToChar(ChessPiece::WhiteRook), 'R');
}

TEST(CharToChessPieceTest, ValidInput)
{
  ASSERT_EQ(charToChessPiece('r'), ChessPiece::BlackRook);
  ASSERT_EQ(charToChessPiece('R'), ChessPiece::WhiteRook);
}

TEST(CharToChessPieceTest, InvalidInput)
{
  ASSERT_THROW(charToChessPiece('$'), std::invalid_argument);
  ASSERT_THROW(charToChessPiece('a'), std::invalid_argument);
}

TEST(PieceColorTest, ValidInput)
{
  ASSERT_EQ(getPieceColor(ChessPiece::BlackQueen), PieceColor::Black);
  ASSERT_EQ(getPieceColor(ChessPiece::BlackKing), PieceColor::Black);
  ASSERT_EQ(getPieceColor(ChessPiece::WhiteQueen), PieceColor::White);
  ASSERT_EQ(getPieceColor(ChessPiece::WhiteKing), PieceColor::White);
}

TEST(PieceColorTest, InvalidInput) { ASSERT_THROW(getPieceColor(ChessPiece::Empty), std::invalid_argument); }

TEST(AlgebraicToIndexTest, ValidInput)
{
  ASSERT_EQ(algebraicToIndex("a8"), 0);
  ASSERT_EQ(algebraicToIndex("h8"), 7);
  ASSERT_EQ(algebraicToIndex("a1"), 56);
  ASSERT_EQ(algebraicToIndex("h1"), 63);
  ASSERT_EQ(algebraicToIndex("d5"), 27);
}

TEST(AlgebraicToIndexTest, InvalidInput)
{
  ASSERT_THROW(algebraicToIndex(""), std::invalid_argument);
  ASSERT_THROW(algebraicToIndex("a"), std::invalid_argument);
  ASSERT_THROW(algebraicToIndex("a77"), std::invalid_argument);

  ASSERT_THROW(algebraicToIndex("`1"), std::out_of_range);
  ASSERT_THROW(algebraicToIndex("i2"), std::out_of_range);
  ASSERT_THROW(algebraicToIndex("@3"), std::out_of_range);
  ASSERT_THROW(algebraicToIndex("I4"), std::out_of_range);
  ASSERT_THROW(algebraicToIndex("A0"), std::out_of_range);
  ASSERT_THROW(algebraicToIndex("H9"), std::out_of_range);
}

TEST(IndexToAlgebraicTest, ValidInput)
{
  ASSERT_EQ(indexToAlgebraic(0), "a8");
  ASSERT_EQ(indexToAlgebraic(7), "h8");
  ASSERT_EQ(indexToAlgebraic(56), "a1");
  ASSERT_EQ(indexToAlgebraic(63), "h1");
  ASSERT_EQ(indexToAlgebraic(27), "d5");
}

TEST(IndexToAlgebraicTest, InvalidInput)
{
  ASSERT_THROW(indexToAlgebraic(-1), std::out_of_range);
  ASSERT_THROW(indexToAlgebraic(64), std::out_of_range);
}

TEST(PiecePlacementStringToArrayTest, ValidInput)
{
  PiecePlacement expected = startingPiecePlacement;
  PiecePlacement actual = piecePlacementStringToArray(startingPiecePlacementString);
  ASSERT_EQ(actual, expected);
}

TEST(PiecePlacementStringToArrayTest, InvalidInput)
{
  std::string test_string = startingPiecePlacementString + "K";
  ASSERT_THROW(piecePlacementStringToArray(test_string), std::invalid_argument);
  ASSERT_THROW(piecePlacementStringToArray("rnbkqbnr"), std::invalid_argument);

  ASSERT_THROW(piecePlacementStringToArray("rnbqkbnr/appppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"), std::invalid_argument);
}

TEST(PiecePlacementArrayToStringTest, ValidInput)
{
  std::string expected = startingPiecePlacementString;
  std::string actual = piecePlacementArrayToString(startingPiecePlacement);
  ASSERT_EQ(actual, expected);
}

TEST(ParseCastlingAvailabilityTest, ValidInput)
{
  CastlingAvailability expected({false, false, false, false});

  expected.blackLong = true;
  ASSERT_EQ(parseCastlingAvailability("q"), expected);

  expected.blackShort = true;
  expected.whiteLong = true;
  expected.whiteShort = true;
  ASSERT_EQ(parseCastlingAvailability("KQkq"), expected);
}

TEST(ParseCastlingAvailabilityTest, InvalidInput)
{
  ASSERT_THROW(parseCastlingAvailability("&"), std::invalid_argument);
  ASSERT_THROW(parseCastlingAvailability("Kk$"), std::invalid_argument);
  ASSERT_THROW(parseCastlingAvailability("KQkqb"), std::invalid_argument);
}

TEST(CastlingAvailabilityToString, ValidInput)
{
  ASSERT_EQ(castlingAvailabilityToString(CastlingAvailability({true, true, true, true})), "KQkq");
  ASSERT_EQ(castlingAvailabilityToString(CastlingAvailability({false, false, false, true})), "q");
}

TEST(IndexToFileRank, ValidInput)
{
  FileRank expected;

  expected = {1, 8};
  ASSERT_EQ(indexToFileRank(BoardIndex(0)), expected);
  expected = {8, 8};
  ASSERT_EQ(indexToFileRank(BoardIndex(7)), expected);
  expected = {1, 1};
  ASSERT_EQ(indexToFileRank(BoardIndex(56)), expected);
  expected = {8, 1};
  ASSERT_EQ(indexToFileRank(BoardIndex(63)), expected);
  expected = {3, 6};
  ASSERT_EQ(indexToFileRank(BoardIndex(18)), expected);
}

TEST(FileRankToIndex, ValidInput)
{
  ASSERT_EQ(fileRankToIndex({1, 1}), 56);
  ASSERT_EQ(fileRankToIndex({1, 8}), 0);
  ASSERT_EQ(fileRankToIndex({8, 1}), 63);
  ASSERT_EQ(fileRankToIndex({8, 8}), 7);
  ASSERT_EQ(fileRankToIndex({3, 6}), 18);
}

TEST(FileRankToIndex, InvalidInput)
{
  ASSERT_THROW(fileRankToIndex({0, 1}), std::out_of_range);
  ASSERT_THROW(fileRankToIndex({1, 0}), std::out_of_range);
  ASSERT_THROW(fileRankToIndex({9, 1}), std::out_of_range);
  ASSERT_THROW(fileRankToIndex({1, 9}), std::out_of_range);
}
