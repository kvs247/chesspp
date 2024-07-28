#include <iostream>
#include <fstream>
#include <vector>
#include "draw.h"

void draw(char pieces[64])
{
  int col_mult = 8; // must be even
  int row_mult = 4; // must be even

  char c;
  size_t piece_index = 0;
  for (int row = 0; row <= 8 * row_mult; ++row)
  {
    for (int col = 0; col <= 8 * col_mult; ++col)
    {
      bool draw_row = row % row_mult == 0;
      bool draw_col = col % col_mult == 0;
      bool draw_piece = col % (col_mult / 2) == 0 && row % (row_mult / 2) == 0;

      if (draw_row && draw_col)
        c = '+';
      // horizontal lines
      else if (draw_row)
        c = '-';
      // vertical lines
      else if (draw_col)
        c = '|';
      // pieces
      else if (draw_piece)
      {
        c = pieces[piece_index] ? pieces[piece_index] : ' ';
        ++piece_index;
      }
      else
        c = ' ';
      std::cout << c;
    }
    std::cout << '\n';
  }
}

void clear_screen()
{
  std::cout << "\033[2J\033[H";
}