#pragma once

#include <fstream>
#include <iostream>
#include <vector>

#include "types.hpp"
#include "utils.hpp"

using std::cout;

int BORDER_WIDTH = 3; // at least 2

void draw(PiecePlacement pieces, const std::string &message)
{
  int colMult = 8; // must be even
  int rowMult = 4; // must be even

  char c;
  size_t pieceIndex = 0;
  for (int row = 0; row <= 8 * rowMult; ++row)
  {
    // ranks
    cout << ' ';
    if ((row + rowMult / 2) % rowMult == 0)
      cout << 8 - row / rowMult << std::string(BORDER_WIDTH - 2, ' ');
    else
      cout << std::string(BORDER_WIDTH - 1, ' ');

    for (int col = 0; col <= 8 * colMult; ++col)
    {
      bool drawRow = row % rowMult == 0;
      bool drawCol = col % colMult == 0;
      bool drawPiece = col % (colMult / 2) == 0 && row % (rowMult / 2) == 0;

      if (drawRow && drawCol)
        c = '+';
      // horizontal lines
      else if (drawRow)
        c = '-';
      // vertical lines
      else if (drawCol)
        c = '|';
      // pieces
      else if (drawPiece)
      {
        char pieceChar = chessPieceToChar(pieces[pieceIndex]);
        c = pieceChar ? pieceChar : ' ';
        ++pieceIndex;
      }
      else
        c = ' ';
      cout << c;
    }
    cout << '\n';
  }

  // files
  cout << std::string(BORDER_WIDTH / 2 - 1, '\n');
  cout << std::string(BORDER_WIDTH, ' ');
  for (int i = 0; i <= 8 * colMult + 2; i++)
  {
    if ((i + colMult / 2) % colMult == 0)
    {
      char c = i / colMult + 'A';
      cout << c;
    }
    else
      cout << ' ';
  }

  cout << "\n\n";
  size_t messageSize = message.size();
  if (messageSize > 0)
  {
    int boardWidth = 8 * colMult / 2;
    int msgOffset = messageSize / 2 + messageSize % 2;
    int numSpace = BORDER_WIDTH + boardWidth - msgOffset;
    cout << std::string(numSpace, ' ') << "~" << message
         << ((messageSize % 2 == 0) ? " " : "~") << "\n\n";
  }
}

void clearScreen() { cout << "\033[2J\033[H"; }