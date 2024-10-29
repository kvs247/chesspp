# To-Do

- `const` everything that is not modified (class functions as well)
- Somewhat duplicated logic with check logic, e.g. knight_offsets are copied from piece.cpp
- in file rank usage (piece.hpp & utils.cpp), rank meaning is inconsistent with traditional chess lingo
- piece_color() in utils.hpp returns White (or maybe Black) when a non-piece is passed.... not great prob
- make portable (cmake)
- clear log/txt on new instance
- need to update unit tests :(
  - [ ] castling
  - [ ] pawn promotion
- better understand logger and config
