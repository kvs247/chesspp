#include <iostream>
#include <ncurses.h>
#include "render.h"

using namespace std;

int main()
{
  string in = "";
  while (1)
  {
    draw(in);
    cin >> in; 
  }

  return 0;
}