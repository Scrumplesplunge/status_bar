#include "buffer.h"
#include "config.h"
#include "executor.h"
#include "set_status.h"
#include "uptime.h"
#include "volume.h"
#include "wall_time.h"

#include <chrono>
#include <cstdio>
#include <iostream>

using namespace std::chrono_literals;
using namespace std;

struct StatusBuffers {
  FixedBuffer<32> uptime;
  FixedBuffer<8> volume;
  FixedBuffer<32> wall_time;
};

class UpdateStatus : public Task {
 public:
  UpdateStatus(StatusBuffers* buffers)
      : buffers_(buffers) {}

  void Perform(Executor*) override {
    FixedBuffer<256> status;
    snprintf(status.get(), status.size(),
             "[Volume %s]  [Uptime %s | %s]",
             buffers_->volume.get(), buffers_->uptime.get(),
             buffers_->wall_time.get());
    display_.SetStatus(status.get());
  }

 private:
  StatusBuffers* buffers_;

  DisplayHandle display_;
};

int main(int argc, char* args[]) {
  if (argc < 2) {
    std::cerr << "Usage: status_bar <config file>\n";
    return 1;
  }

  Config config(args[1]);

  StatusBuffers buffers;
  UpdateStatus update_status(&buffers);

  // System uptime.
  CalculateUptime calculate_uptime(&buffers.uptime, &update_status);
  PeriodicTask uptime_calculator(&calculate_uptime, 250ms);

  // Audio volume.
  CalculateVolume calculate_volume(config, &buffers.volume, &update_status);
  PeriodicTask volume_calculator(&calculate_volume, 50ms);

  // Wall (civil) time.
  CalculateWallTime calculate_wall_time(
      config, &buffers.wall_time, &update_status);
  PeriodicTask wall_time_calculator(&calculate_wall_time, 1s);

  Executor executor;
  executor.Schedule(&uptime_calculator);
  executor.Schedule(&volume_calculator);
  executor.Schedule(&wall_time_calculator);
  executor.Run();
}
