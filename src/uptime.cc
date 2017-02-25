#include "uptime.h"

#include <cstdio>
#include <sys/sysinfo.h>
#include <stdexcept>

CalculateUptime::CalculateUptime(Buffer* buffer, Task* on_update)
    : buffer_(buffer), on_update_(on_update) {}

void CalculateUptime::Perform(Executor* executor) {
  struct sysinfo info;
  int error = sysinfo(&info);
  if (error != 0) throw std::runtime_error("Could not fetch sysinfo().");

  // Separate the uptime into different units.
  uint64_t seconds = info.uptime;
  uint64_t minutes = seconds / 60;
  uint64_t hours = minutes / 60;
  uint64_t days = hours / 24;

  // Discard values larger than the modulus for each type as they are accounted
  // for by the next unit up.
  hours %= 24;
  minutes %= 60;
  seconds %= 60;

  // Compute the string form.
  int i = 0;
  auto append = [&](int value, const char* suffix) {
    i += snprintf(buffer_->get() + i, buffer_->size() - i,
                  "%u%s ", value, suffix);
  };

  if (days > 0) append(days, "d");
  if (hours > 0) append(hours, "h");
  if (minutes > 0) append(minutes, "m");
  if (seconds > 0 || i == 0) append(seconds, "s");

  // Each append finishes with a space. Remove the space.
  buffer_->at(i - 1) = '\0';

  executor->Schedule(on_update_);
}
