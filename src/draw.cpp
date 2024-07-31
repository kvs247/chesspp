#include <iostream>
#include <fstream>
#include <vector>
#include "draw.h"

using std::cout;

int BORDER_WIDTH = 3; // at least 2

void draw(std::array<char, 64> pieces, const std::string &message)
{
  int col_mult = 8; // must be even
  int row_mult = 4; // must be even

  char c;
  size_t piece_index = 0;
  for (int row = 0; row <= 8 * row_mult; ++row)
  {
    // ranks
    cout << ' ';
    if ((row + row_mult / 2) % row_mult == 0)
      cout << 8 - row / row_mult << std::string(BORDER_WIDTH - 2, ' ');
    else
      cout << std::string(BORDER_WIDTH - 1, ' ');

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
      cout << c;
    }
    cout << '\n';
  }

  // files
  cout << std::string(BORDER_WIDTH / 2 - 1, '\n');
  cout << std::string(BORDER_WIDTH, ' ');
  for (int i = 0; i <= 8 * col_mult + 2; i++)
  {
    if ((i + col_mult / 2) % col_mult == 0)
    {
      char c = i / col_mult + 'A';
      cout << c;
    }
    else
      cout << ' ';
  }

  cout << "\n\n";
  if (message.size() > 0)
    cout << std::string(BORDER_WIDTH, ' ') << "~" << message << "\n\n";
}

void clear_screen()
{
  cout << "\033[2J\033[H";
}