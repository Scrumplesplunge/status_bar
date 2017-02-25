#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <X11/Xlib.h>

using namespace std::chrono_literals;
using namespace std;

void SetStatus(const char* status) {
  Display* display = XOpenDisplay(nullptr);
  if (display == nullptr) {
    std::cerr << "Fatal: Could not open display.\n";
    exit(EXIT_FAILURE);
  }
  XStoreName(display, DefaultRootWindow(display), status);
  XSync(display, 0);
  XCloseDisplay(display);
}

int main() {
  const int MAX_LENGTH = 256;
  char status[MAX_LENGTH];
  
  while (true) {
    snprintf(status, MAX_LENGTH, "Hello, World!");

    SetStatus(status);

    this_thread::sleep_for(100ms);
  }
}
