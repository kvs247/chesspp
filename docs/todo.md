# To-Do

- `const` class method functions
- Somewhat duplicated logic with check logic, e.g. knight_offsets are copied from piece.cpp
- switch to camelCase
- in file rank usage (piece.hpp & utils.cpp), rank meaning is inconsistent with traditional chess lingo
- consider Piece::legal_square_indexes should check for friendly fire... probably should
- piece_color() in utils.hpp returns White (or maybe Black) when a non-piece is passed.... not great prob
- eventually get std::out_of_range throw with cpu vs cpu
- make portable (cmake)
- clear log/txt on new instance
- need to update unit tests :(
  - [ ] castling
  - [ ] pawn promotion
- better understand logger and config
