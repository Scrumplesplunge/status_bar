#include "wall_time.h"

#include <ctime>
#include <map>
#include <tuple>

CalculateWallTime::CalculateWallTime(
    const Config& config, Buffer* buffer, Task* on_update)
    : buffer_(buffer), on_update_(on_update),
      format_(config.Get("wall_time", "format", "%Y-%m-%d %A %H:%M")) {}

void CalculateWallTime::Perform(Executor* executor) {
  // Get the current time.
  time_t t = time(nullptr);
  tm* date = localtime(&t);
  strftime(buffer_->get(), buffer_->size(), format_.c_str(), date);

  executor->Schedule(on_update_);
}
