#include "../src/constants.hpp"
#include "../src/utils.hpp"

#include "../src/types.hpp"

#include <array>
#include <gtest/gtest.h>

TEST(IsChessPieceTest, ValidInput)
{
  ASSERT_TRUE(is_chess_piece('q'));
  ASSERT_TRUE(is_chess_piece('Q'));
}

TEST(IsChessPieceTest, InvalidInput)
{
  ASSERT_FALSE(is_chess_piece('a'));
  ASSERT_FALSE(is_chess_piece('%'));
}

TEST(ColorToCharTest, ValidInput)
{
  ASSERT_EQ(color_to_char(PieceColor::Black), 'b');
  ASSERT_EQ(color_to_char(PieceColor::White), 'w');
}

TEST(CharToColorTest, ValidInput)
{
  ASSERT_EQ(char_to_Color('w'), PieceColor::White);
  ASSERT_EQ(char_to_Color('b'), PieceColor::Black);
}

TEST(CharToColorTest, InvalidInput)
{
  ASSERT_THROW(char_to_Color('k'), std::invalid_argument);
  ASSERT_THROW(char_to_Color('&'), std::invalid_argument);
}

TEST(ChessPieceToCharTest, ValidInput)
{
  ASSERT_EQ(chessPiece_to_char(ChessPiece::BlackRook), 'r');
  ASSERT_EQ(chessPiece_to_char(ChessPiece::WhiteRook), 'R');
}

TEST(CharToChessPieceTest, ValidInput)
{
  ASSERT_EQ(char_to_ChessPiece('r'), ChessPiece::BlackRook);
  ASSERT_EQ(char_to_ChessPiece('R'), ChessPiece::WhiteRook);
}

TEST(CharToChessPieceTest, InvalidInput)
{
  ASSERT_THROW(char_to_ChessPiece('$'), std::invalid_argument);
  ASSERT_THROW(char_to_ChessPiece('a'), std::invalid_argument);
}

TEST(PieceColorTest, ValidInput)
{
  ASSERT_EQ(piece_color(ChessPiece::BlackQueen), PieceColor::Black);
  ASSERT_EQ(piece_color(ChessPiece::BlackKing), PieceColor::Black);
  ASSERT_EQ(piece_color(ChessPiece::WhiteQueen), PieceColor::White);
  ASSERT_EQ(piece_color(ChessPiece::WhiteKing), PieceColor::White);
}

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

  ASSERT_THROW(algebraic_to_index("`1"), std::out_of_range);
  ASSERT_THROW(algebraic_to_index("i2"), std::out_of_range);
  ASSERT_THROW(algebraic_to_index("@3"), std::out_of_range);
  ASSERT_THROW(algebraic_to_index("I4"), std::out_of_range);
  ASSERT_THROW(algebraic_to_index("A0"), std::out_of_range);
  ASSERT_THROW(algebraic_to_index("H9"), std::out_of_range);
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
  PiecePlacement expected = starting_piece_placement;
  PiecePlacement actual = piece_placement_string_to_array(starting_piece_placement_string);
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

TEST(IndexToFileRank, ValidInput)
{
  FileRank expected;

  expected = {1, 8};
  ASSERT_EQ(index_to_file_rank(BoardIndex(0)), expected);
  expected = {8, 8};
  ASSERT_EQ(index_to_file_rank(BoardIndex(7)), expected);
  expected = {1, 1};
  ASSERT_EQ(index_to_file_rank(BoardIndex(56)), expected);
  expected = {8, 1};
  ASSERT_EQ(index_to_file_rank(BoardIndex(63)), expected);
  expected = {3, 6};
  ASSERT_EQ(index_to_file_rank(BoardIndex(18)), expected);
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
