#pragma once

#include "types.hpp"

#include <array>
#include <string>

inline const std::string starting_piece_placement_string = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

inline const PiecePlacement starting_piece_placement = {
    ChessPiece::BlackRook, ChessPiece::BlackKnight, ChessPiece::BlackBishop, ChessPiece::BlackQueen, ChessPiece::BlackKing, ChessPiece::BlackBishop, ChessPiece::BlackKnight, ChessPiece::BlackRook,
    ChessPiece::BlackPawn, ChessPiece::BlackPawn, ChessPiece::BlackPawn, ChessPiece::BlackPawn, ChessPiece::BlackPawn, ChessPiece::BlackPawn, ChessPiece::BlackPawn, ChessPiece::BlackPawn,
    ChessPiece::Empty, ChessPiece::Empty, ChessPiece::Empty, ChessPiece::Empty, ChessPiece::Empty, ChessPiece::Empty, ChessPiece::Empty, ChessPiece::Empty,
    ChessPiece::Empty, ChessPiece::Empty, ChessPiece::Empty, ChessPiece::Empty, ChessPiece::Empty, ChessPiece::Empty, ChessPiece::Empty, ChessPiece::Empty,
    ChessPiece::Empty, ChessPiece::Empty, ChessPiece::Empty, ChessPiece::Empty, ChessPiece::Empty, ChessPiece::Empty, ChessPiece::Empty, ChessPiece::Empty,
    ChessPiece::Empty, ChessPiece::Empty, ChessPiece::Empty, ChessPiece::Empty, ChessPiece::Empty, ChessPiece::Empty, ChessPiece::Empty, ChessPiece::Empty,
    ChessPiece::WhitePawn, ChessPiece::WhitePawn, ChessPiece::WhitePawn, ChessPiece::WhitePawn, ChessPiece::WhitePawn, ChessPiece::WhitePawn, ChessPiece::WhitePawn, ChessPiece::WhitePawn,
    ChessPiece::WhiteRook, ChessPiece::WhiteKnight, ChessPiece::WhiteBishop, ChessPiece::WhiteQueen, ChessPiece::WhiteKing, ChessPiece::WhiteBishop, ChessPiece::WhiteKnight, ChessPiece::WhiteRook};

inline const std::string starting_castling_availability = "KQkq";