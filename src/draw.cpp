#include <iostream>
#include <fstream>
#include <vector>

char pieces[64] = {
    'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',
    'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
    'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'};

int main()
{
  std::ofstream logFile;
  logFile.open("log.txt");
  if (!logFile.is_open())
  {
    std::cout << "Error opening log file";
  }

  unsigned col_mult = 8; // must be even
  unsigned row_mult = 4; // must be even
  unsigned x_max = 8 * col_mult;
  unsigned y_max = 8 * row_mult;

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

  logFile.close();
}