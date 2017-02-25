#include "set_status.h"

#include <cstdlib>
#include <stdexcept>

DisplayHandle::DisplayHandle() {
  display_ = XOpenDisplay(nullptr);
  if (display_ == nullptr)
    throw std::runtime_error("Could not open X display.");
}

DisplayHandle::~DisplayHandle() {
  XCloseDisplay(display_);
}

void DisplayHandle::SetStatus(const char* status) {
  // dwm uses the name of the root window to set the status text. Update the
  // name of the root window.
  XStoreName(display_, DefaultRootWindow(display_), status);
  // Flush the request buffer.
  XSync(display_, 0);
}
