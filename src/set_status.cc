#include "set_status.h"

#include <cstdlib>
#include <iostream>
#include <X11/Xlib.h>

void SetStatus(const char* status) {
  Display* display = XOpenDisplay(nullptr);
  if (display == nullptr) {
    std::cerr << "Fatal: Could not open display.\n";
    exit(EXIT_FAILURE);
  }
  // dwm uses the name of the root window to set the status text. Update the
  // name of the root window.
  XStoreName(display, DefaultRootWindow(display), status);
  // Flush the request buffer.
  XSync(display, 0);
  XCloseDisplay(display);
}
