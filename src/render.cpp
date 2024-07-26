#include <iostream>
#include <ncurses.h>
#include <stdlib.h>
#include <string>
#include "cursor.h"

using std::cout;
using std::flush;
using std::string;
using std::to_string;

const unsigned SQUARE_HEIGHT = 4;
const unsigned SQUARE_WIDTH = 8;
const unsigned TOP_PAD = 8;
const unsigned LEFT_PAD = 16;

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

  string render;

  // get cursor position
  int cursor_x = cursor.x * cols;
  int cursor_y = cursor.y * rows;
  string cursor_x_str = to_string(cursor_x);
  string cursor_y_str = to_string(cursor_y);

  // heading info for development
  string heading = '(' + cursor_x_str + ", " + cursor_y_str + ')';
  heading += '\n';
  render += heading;

  for (int i = 0; i != cursor_y - 2; i++)
  {
    render += '\n';
  }

  for (int i = 0; i != cursor_x + 1; i++)
  {
    render += ' ';
  }
  render += '$';



  cout << render << flush;
}

// board with '#' and 'H'
// void draw(const Point cursor)
// {
//   string render;
//   render += "(" + to_string(cursor.x) + ", " + to_string(cursor.y) + ")\n";
//   string top(TOP_PAD, '\n');
//   render += top;
//   string left(LEFT_PAD, ' ');

//   string dark_square_row(SQUARE_WIDTH, '#');
//   string light_square_row(SQUARE_WIDTH, 'H');

//   int sign;
//   for (int row = 0; row != 8; row++)
//   {
//     sign = (row % 2) ? 0 : 1;
//     for (int k = 0; k != SQUARE_HEIGHT; k++)
//     {
//       render += left;
//       for (int col = 0; col != 8; col++)
//       {
//         render += ((col % 2 == sign) ? light_square_row : dark_square_row);
//       }
//       render += '\n';
//     }
//   }

//   cout << render << flush;
// }

// window border plus draw '#' at curosor
// void draw(const Point cursor)
// {
//   // Initialize ncurses
//   initscr();
//   cbreak();
//   noecho();

//   // Get window size
//   int rows, cols;
//   getmaxyx(stdscr, rows, cols);
//   rows--;
//   // cols++;

//   // End ncurses mode
//   endwin();

//   string render((cols + 1) * rows, ' '); // (cols + 1) to account for newline
//   for (int i = 1; i < rows; i++)
//     render[(cols + 1) * i - 1] = '\n';

//   char border = '~';
//   for (int i = 0; i < rows; i++)
//   {
//     for (int j = 0; j < cols; j++)
//     {
//       if (j == 0 || j == cols - 1)
//       {
//         render[(cols + 1) * i + j] = border;
//         continue;
//       }
//       if (i == 0 || i == rows - 2)
//       {
//         if (j % 2 == 0)
//           render[(cols + 1) * i + j] = border;
//         continue;
//       }
//     }
//   }

//   int cursor_x = cursor.x * cols;
//   int cursor_y = cursor.y * rows;
//   render[(cols + 1) * cursor_y + cursor_x] = '#';

//   cout << render;
// }
