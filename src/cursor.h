#ifndef CURSOR_H
#define CURSOR_H

#include <X11/Xlib.h>

extern Display *display;

typedef struct Point
{
  double x;
  double y;
} Point;

void clearScreen();

void xServerConnect();

void xServerDisconnect();

Point getCursorPoint();

#endif
