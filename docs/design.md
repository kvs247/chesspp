# Design/Features

## Core Features

- [ ] game logic
  - [x] find piece legal moves
    - [x] pawn
    - [x] knight
    - [x] bishop
    - [x] rook
    - [x] king
    - [x] queen
  - endgame scenarios
    - [x]  checkmate
    - [ ]  resignation
    - [x]  timeout
    - [ ]  draw
      - [x]  stalemate
      - [x]  50-move rule
      - [x]  repetition
      - [ ]  agreement
      - [x]  insufficient material
        - [x]  king vs king
        - [x]  king + minor piece vs king
        - [x]  king + two knights vs king
        - [x]  king + minor piece vs king + minor piece
        - [x]  timeout vs insufficient material
  - [x] other
    - [x] castling
    - [x] pawn promotion
      - [x] choose piece
    - [x] check
- [ ] display
  - [x] draw board & pieces
  - [x] moves played list
    - [x] piece + to square
    - [x] capture
    - [x] ambiguous piece
    - [x] castle
    - [x] check/checkmate
  - [ ] help menu
- [x] CPU opponent
  - [x] random moves
- [x] time control
- [x] logging

## Strech Features

- [ ] GUI
- [ ] Sophisticated CPU opponent (LLM or lichess API)
- [ ] OS portability
- [ ] multiplayer over internet
