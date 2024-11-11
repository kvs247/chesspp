# To-Do

- Somewhat duplicated logic with check logic, e.g. knight_offsets are copied from piece.cpp
- in file rank usage (piece.hpp & utils.cpp), rank meaning is inconsistent with traditional chess lingo
- piece_color() in utils.hpp returns White (or maybe Black) when a non-piece is passed.... not great prob *****
- better understand logger and config (singleton)
- rename/make clearer intent of Piece::linearSquareIndexes & Piece::squareIndexes
- move list notation for checkmate
- issue with knight ambiguous move. Guess it it will always specify column and rank when only one is necessary (unlike bishop & rook?)
- tests
  - draw.hpp
- rework Game::processMove() to take a fromIndex & toIndex. this will allow better testing (in particulate GameHandleGameOver)
- refactors
  - game over logic
