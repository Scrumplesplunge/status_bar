#include "set_status.h"

#include <chrono>
#include <thread>

using namespace std::chrono_literals;
using namespace std;

int main() {
  const int MAX_LENGTH = 256;
  char status[MAX_LENGTH];
  
  DisplayHandle display;

  while (true) {
    snprintf(status, MAX_LENGTH, "Hello, World!");
    display.SetStatus(status);
    this_thread::sleep_for(100ms);
  }
}
