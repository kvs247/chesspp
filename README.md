# Chesspp - A CLI Chess Application in C++

## Overview

A command-line chess application showcasing modern C++ practices, multithreading, and clean architecture. Built with C++17, featuring a complete chess engine implementation and CPU opponent.

## Technical Features

- Multithreaded architecture for game clock and input handling
- Smart pointer implementation for memory management
- Unit testing using Google Test

## Chess Features

- Complete rule implementation including en passant, castling, and draw conditions
- CPU opponent with random move generation
- Move history in algebraic notation
- Configurable time control clock
- ASCII board visualization and UI

## Build & Run

``` bash
git clone https://github.com/kvs247/chesspp.git
cd chesspp
mkdir build && cd build
cmake ..
make
./chess
```

## Requirements

- C++17 compatible compiler
- CMake 3.28+

## Testing

``` bash
ctest
```
