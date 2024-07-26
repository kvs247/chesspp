#include <ncurses.h>

#define WIDTH 30
#define HEIGHT 10

int startx = 0;
int starty = 0;

char *choices[] = {
    "Choice 1",
    "Choice 2",
    "Choice 3",
    "Choice 4",
    "Exit",
};

int n_choices = sizeof(choices) / sizeof(char *);

void print_menu(WINDOW *menu_win, int highlight);

int draw2()
{
  WINDOW *menu_win;
  MEVENT event;

  initscr();
  clear();
  noecho();
  cbreak();

  startx = (80 - WIDTH) / 2;
  starty = (24 - HEIGHT) / 2;

  attron(A_REVERSE);
  refresh();
  attroff(A_REVERSE);

  menu_win = newwin(HEIGHT, WIDTH, starty, startx);
  print_menu(menu_win, 1);
  mousemask(ALL_MOUSE_EVENTS, NULL);

  // mvprintw("hellol");

  while (true)
  {
    int c = wgetch(menu_win);
    switch (c)
    {
    case KEY_MOUSE:
      if (getmouse(&event) == OK)
      {
        if (event.bstate & BUTTON1_PRESSED)
        {
          mvwprintw(menu_win, 0, 0, "clicked (%d, %d)\n", event.x, event.y);
          wrefresh(menu_win);
        }
      }
    }
  }

  endwin(); // end curses mode

  return 0;
}

void print_menu(WINDOW *menu_win, int highlight)
{
  int x, y, i;

  x = 2;
  y = 2;
  box(menu_win, 0, 0);
  for (i = 0; i < n_choices; ++i)
  {
    if (highlight == i + 1)
    {
      wattron(menu_win, A_REVERSE);
      mvwprintw(menu_win, y, x, "%s", choices[i]);
      wattroff(menu_win, A_REVERSE);
    }
    else
      mvwprintw(menu_win, y, x, "%s", choices[i]);
    ++y;
  }
  wrefresh(menu_win);
}