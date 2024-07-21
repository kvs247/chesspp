#include <iostream>
#include <ncurses.h>
#include <unistd.h>
#include "render.h"
#include "cursor.h"

using namespace std;

int main()
{
  xServerConnect();

  Point cursorPoint;

  string in = "";
  while (true)
  {
    clearScreen();
    
    cursorPoint = getCursorPoint();
    draw(cursorPoint);

    usleep(10000);
  }

  xServerDisconnect();

  return 0;
}