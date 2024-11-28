#pragma once

#include <string>

#include "types.hpp"

inline const std::string startingPiecePlacementString = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

inline const std::string startingFenString = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

inline const PiecePlacement startingPiecePlacement = {
    ChessPiece::BlackRook, ChessPiece::BlackKnight, ChessPiece::BlackBishop, ChessPiece::BlackQueen,
    ChessPiece::BlackKing, ChessPiece::BlackBishop, ChessPiece::BlackKnight, ChessPiece::BlackRook,
    ChessPiece::BlackPawn, ChessPiece::BlackPawn,   ChessPiece::BlackPawn,   ChessPiece::BlackPawn,
    ChessPiece::BlackPawn, ChessPiece::BlackPawn,   ChessPiece::BlackPawn,   ChessPiece::BlackPawn,
    ChessPiece::Empty,     ChessPiece::Empty,       ChessPiece::Empty,       ChessPiece::Empty,
    ChessPiece::Empty,     ChessPiece::Empty,       ChessPiece::Empty,       ChessPiece::Empty,
    ChessPiece::Empty,     ChessPiece::Empty,       ChessPiece::Empty,       ChessPiece::Empty,
    ChessPiece::Empty,     ChessPiece::Empty,       ChessPiece::Empty,       ChessPiece::Empty,
    ChessPiece::Empty,     ChessPiece::Empty,       ChessPiece::Empty,       ChessPiece::Empty,
    ChessPiece::Empty,     ChessPiece::Empty,       ChessPiece::Empty,       ChessPiece::Empty,
    ChessPiece::Empty,     ChessPiece::Empty,       ChessPiece::Empty,       ChessPiece::Empty,
    ChessPiece::Empty,     ChessPiece::Empty,       ChessPiece::Empty,       ChessPiece::Empty,
    ChessPiece::WhitePawn, ChessPiece::WhitePawn,   ChessPiece::WhitePawn,   ChessPiece::WhitePawn,
    ChessPiece::WhitePawn, ChessPiece::WhitePawn,   ChessPiece::WhitePawn,   ChessPiece::WhitePawn,
    ChessPiece::WhiteRook, ChessPiece::WhiteKnight, ChessPiece::WhiteBishop, ChessPiece::WhiteQueen,
    ChessPiece::WhiteKing, ChessPiece::WhiteBishop, ChessPiece::WhiteKnight, ChessPiece::WhiteRook};

inline const CastlingAvailability startingCastlingAvailability({true, true, true, true});

inline const std::string shortCastleString = "O-O";
inline const std::string longCastleString = "O-O-O";