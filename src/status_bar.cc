#include "set_status.h"

#include <chrono>
#include <thread>

using namespace std::chrono_literals;
using namespace std;

int main() {
  const int MAX_LENGTH = 256;
  char status[MAX_LENGTH];
  
  while (true) {
    snprintf(status, MAX_LENGTH, "Hello, World!");
    SetStatus(status);
    this_thread::sleep_for(100ms);
  }
}
