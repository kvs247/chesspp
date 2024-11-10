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
    - [ ]  timeout
    - [ ]  draw
      - [x]  stalemate
      - [x]  50-move rule
      - [ ]  repetition
      - [ ]  agreement
      - [ ]  insufficient material
        - [ ]  king vs king
        - [ ]  king + minor piece vs king
        - [ ]  king + two knights vs king
        - [ ]  king + minor piece vs king + minor piece
        - [ ]  lone king vs all the pieces (timeout vs insufficient material)
  - [ ] other
    - [x] castling
    - [x] pawn promotion
      - [ ] choose piece
    - [x] check
- [ ] display
  - [x] draw board & pieces
  - [x] moves played list
    - [x] piece + to square
    - [x] capture
    - [x] ambiguous piece
    - [x] castle
    - [x] check/checkmate
  - [ ] clock
  - [ ] help menu
- [x] CPU opponent
  - [x] random moves
- [ ] time control
- [x] logging

## Strech Features

- [ ] GUI
- [ ] Sophisticated CPU opponent (LLM)
- [ ] OS portability
- [ ] connect to network
