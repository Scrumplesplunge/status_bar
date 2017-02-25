#include "executor.h"
#include "set_status.h"

#include <chrono>
#include <cstdio>

using namespace std::chrono_literals;
using namespace std;

class UpdateStatus : public Task {
 public:
  void Perform(Executor*) override {
    snprintf(status_, MAX_LENGTH, "Hello, World!");
    display_.SetStatus(status_);
  }

 private:
  static const int MAX_LENGTH = 256;
  char status_[MAX_LENGTH];
  DisplayHandle display_;
};

int main() {
  UpdateStatus update_status;
  PeriodicTask status_updater(&update_status, 10ms);

  Executor executor;
  executor.Schedule(&status_updater);
  executor.Run();
}
