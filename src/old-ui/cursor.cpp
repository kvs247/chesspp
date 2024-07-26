#include <iostream>
#include <X11/Xlib.h>
#include "cursor.h"

Display *display;

void clearScreen()
{
  // ANSI escape code to clear the screen
  std::cout << "\033[2J\033[H";
}

void xServerConnect()
{
  display = XOpenDisplay(NULL);
  if (display == NULL)
  {
    std::cerr << "Unable to connect to X display\n";
    return;
  }
}

void xServerDisconnect()
{
  XCloseDisplay(display);
}

Point getCursorPoint()
{
  Window window;        // currently focused window ID
  int revert_to_return; // return focus state

  // get the window with input focus (which should be the terminal window)
  XGetInputFocus(display, &window, &revert_to_return);

  Window root_return, child_return;
  int root_x, root_y, win_x, win_y;
  unsigned int mask_return; // state of modifier keys and pointer buttons

  // Query pointer with the focused window (terminal window)
  XQueryPointer(display, window,
                &root_return, &child_return,
                &root_x, &root_y,
                &win_x, &win_y,
                &mask_return);

  // get window attributes
  // PROBLEM: Ubuntu terminal is InputOnly, so size will be (1, 1)
  XWindowAttributes window_attributes;
  XGetWindowAttributes(display, window, &window_attributes);
  double window_width = 1920;  // temp
  double window_height = 1080; // temp

  double cursor_x = win_x / window_width;
  double cursor_y = win_y / window_height;

  return {
      cursor_x,
      cursor_y,
  };
}
