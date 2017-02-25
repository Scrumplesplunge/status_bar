#include "wall_time.h"

#include <ctime>
#include <map>
#include <tuple>

namespace {

const char* weekdays[] = {
  "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday",
};

}  // namespace

CalculateWallTime::CalculateWallTime(Buffer* buffer, Task* on_update)
    : buffer_(buffer), on_update_(on_update) {}

void CalculateWallTime::Perform(Executor* executor) {
  // Get the current time.
  time_t t = time(nullptr);
  tm* date = localtime(&t);

  // Format the time as YYYY-MM-DD Weekday HH:MM.
  snprintf(buffer_->get(), buffer_->size(),
           "%04d-%02d-%02d %s %02d:%02d",
           1900 + date->tm_year, 1 + date->tm_mon, date->tm_mday,
           weekdays[date->tm_wday], date->tm_hour, date->tm_min);

  executor->Schedule(on_update_);
}
