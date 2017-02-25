#include "buffer.h"
#include "executor.h"
#include "set_status.h"
#include "uptime.h"
#include "volume.h"

#include <chrono>
#include <cstdio>

using namespace std::chrono_literals;
using namespace std;

struct StatusBuffers {
  FixedBuffer<32> uptime;
  FixedBuffer<8> volume;
};

class UpdateStatus : public Task {
 public:
  UpdateStatus(StatusBuffers* buffers)
      : buffers_(buffers) {}

  void Perform(Executor*) override {
    FixedBuffer<256> status;
    snprintf(status.get(), status.size(),
             "[Volume %s]  [Uptime %s]",
             buffers_->volume.get(), buffers_->uptime.get());
    display_.SetStatus(status.get());
  }

 private:
  StatusBuffers* buffers_;

  DisplayHandle display_;
};

int main() {
  StatusBuffers buffers;

  UpdateStatus update_status(&buffers);

  CalculateUptime calculate_uptime(&buffers.uptime, &update_status);
  PeriodicTask uptime_calculator(&calculate_uptime, 100ms);

  CalculateVolume calculate_volume(&buffers.volume, &update_status);
  PeriodicTask volume_calculator(&calculate_volume, 50ms);

  Executor executor;
  executor.Schedule(&uptime_calculator);
  executor.Schedule(&volume_calculator);
  executor.Run();
}
