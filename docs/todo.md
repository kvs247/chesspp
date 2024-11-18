# To-Do

- create `isPieceWhite` util
- move all `Game` class members to `State` type
- captured pieces in player info display
- tests
  - draw.hpp
    - test ambiguous move handling
- refactors
  - game over logic
  - isSquareUnderAttack should be able to reuse logic from Piece
  - Piece::linearSquareIndexes() & Piece::squareIndexes()
  - rework Game::processMove() to take a fromIndex & toIndex. this will allow better testing (in particulate GameHandleGameOver)
