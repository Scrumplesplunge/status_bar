#include "executor.h"
#include "set_status.h"
#include "uptime.h"

#include <chrono>
#include <cstdio>

using namespace std::chrono_literals;
using namespace std;

class UpdateStatus : public Task {
 public:
  UpdateStatus(const char* uptime)
      : uptime_(uptime) {}

  void Perform(Executor*) override {
    const int MAX_LENGTH = 256;
    char status[MAX_LENGTH];
    snprintf(status, MAX_LENGTH, "[Uptime %s]", uptime_);
    display_.SetStatus(status);
  }

 private:
  const char* uptime_;
  DisplayHandle display_;
};

int main() {
  char uptime[32];
  CalculateUptime calculate_uptime(uptime, 32);
  PeriodicTask uptime_calculator(&calculate_uptime, 1s);

  UpdateStatus update_status(uptime);
  PeriodicTask status_updater(&update_status, 10ms);

  Executor executor;
  executor.Schedule(&status_updater);
  executor.Schedule(&uptime_calculator);
  executor.Run();
}
