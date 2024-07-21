#include <iostream>
#include <ncurses.h>
#include <stdlib.h>
#include "cursor.h"

using namespace std;

void draw(const Point cursor)
{
  // Initialize ncurses
  initscr();
  cbreak();
  noecho();

  // Get window size
  int rows, cols;
  getmaxyx(stdscr, rows, cols);
  rows--;  
  // cols++;

  // End ncurses mode
  endwin();

  string render((cols + 1) * rows, ' '); // (cols + 1) to account for newline
  for (int i = 1; i < rows; i++)
    render[(cols + 1) * i - 1] = '\n';

  char border = '~';
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      if (j == 0 || j == cols - 1)
      {
        render[(cols + 1) * i + j] = border;
        continue;
      }
      if (i == 0 || i == rows - 1)
      {
        if (j % 2 == 0)
          render[(cols + 1) * i + j] = border;
        continue;
      }
    }
  }

  int cursor_x = cursor.x * cols;
  int cursor_y = cursor.y * rows;
  render[(cols + 1) * cursor_y + cursor_x] = '#';

  cout << render;
}
