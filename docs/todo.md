# To-Do

- `const` class method functions
- Create types for objects used throughout project, e.g. make a BoardIndex type for int [0, 63] and a PieceChar type
  - [x] ChessPiece
  - [x] PieceColor
  - [x] IntRange
  - [x] BoardIndex
  - [x] FileRank
  - [ ] EnPassant?
- Somewhat duplicated logic with check logic, e.g. knight_offsets are copied from piece.cpp
- switch to camelCase
- in file rank usage (piece.hpp & utils.cpp), rank meaning is inconsistent with traditional chess lingo
- consider Piece::legal_square_indexes should check for friendly fire... probably should
- piece_color() in utils.hpp returns White (or maybe Black) when a non-piece is passed.... not great prob
- eventually get std::out_of_range throw with cpu vs cpu
