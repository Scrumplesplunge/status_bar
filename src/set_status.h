#pragma once

#include <X11/Xlib.h>

class DisplayHandle {
 public:
  DisplayHandle();
  ~DisplayHandle();

  void SetStatus(const char* status);

 private:
  Display* display_;
};
