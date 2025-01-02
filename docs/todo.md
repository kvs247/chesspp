# To-Do

- captured pieces in player info display
- move pawn promotion input to modal
- fix testing suite
- refactors
  - game over logic
  - isSquareUnderAttack should be able to reuse logic from Piece
  - Piece::linearSquareIndexes() & Piece::squareIndexes()
  - rework Game::processMove() to take a fromIndex & toIndex. this will allow better testing (in particulate GameHandleGameOver)
