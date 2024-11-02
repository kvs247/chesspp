# To-Do

- `const` everything that is not modified (class functions as well)
- Somewhat duplicated logic with check logic, e.g. knight_offsets are copied from piece.cpp
- in file rank usage (piece.hpp & utils.cpp), rank meaning is inconsistent with traditional chess lingo
- piece_color() in utils.hpp returns White (or maybe Black) when a non-piece is passed.... not great prob *****
- better understand logger and config (singleton)
- update Piece::squareIndexes to pass the color everytime instead of std::optional
- move is_king_in_check() to piece.cpp
- move list
  - notation for check
  - notation for checkmate
- tests
  - castling
  - pawn promotion
  - draw.hpp
