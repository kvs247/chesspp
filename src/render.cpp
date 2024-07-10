#include <iostream>
#include <ncurses.h>

using namespace std;

void draw(const string &s)
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

  for (size_t i = 0; i < s.size(); i++)
  {
    render[(cols + 1) * 5 + i + 5] = s[i];
  }

  cout << render;
}
