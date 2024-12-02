# To-Do

- create `isPieceWhite` util
- move all `Game` class members to `State` type
- captured pieces in player info display
- refactors
  - game over logic
  - isSquareUnderAttack should be able to reuse logic from Piece
  - Piece::linearSquareIndexes() & Piece::squareIndexes()
  - rework Game::processMove() to take a fromIndex & toIndex. this will allow better testing (in particulate GameHandleGameOver)
