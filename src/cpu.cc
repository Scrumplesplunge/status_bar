#include "cpu.h"

#include <fstream>

CalculateCpuUsage::CalculateCpuUsage(Buffer* buffer, Task* on_update)
    : buffer_(buffer), on_update_(on_update) {}

void CalculateCpuUsage::Perform(Executor* executor) {
  std::ifstream file("/proc/stat");
  {
    std::string temp;
    file >> temp;
    if (temp != "cpu")
      throw std::runtime_error("First line of /proc/stat is not cpu.");
  }

  struct {
    int64_t user, nice, system, idle, iowait, irq,
            softirq, steal, guest, guest_nice;
  } cpu;

  file >> cpu.user;
  file >> cpu.nice;
  file >> cpu.system;
  file >> cpu.idle;
  file >> cpu.iowait;
  file >> cpu.irq;
  file >> cpu.softirq;
  file >> cpu.steal;
  file >> cpu.guest;
  file >> cpu.guest_nice;

  // This is based on the calculation from:
  // http://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
  int64_t busy_jiffies =
      cpu.user + cpu.nice + cpu.system + cpu.irq + cpu.softirq + cpu.steal;
  int64_t total_jiffies = busy_jiffies + cpu.idle + cpu.iowait;

  // Compute the average usage over the duration of the ring buffer.
  int64_t delta_busy = busy_jiffies - busy_jiffies_[index_];
  int64_t delta_total = total_jiffies - total_jiffies_[index_];
  double usage = 100.0 * delta_busy / delta_total;
  snprintf(buffer_->get(), buffer_->size(), "%.1lf%%", usage);

  // Update the ring buffer entries.
  busy_jiffies_[index_] = busy_jiffies;
  total_jiffies_[index_] = total_jiffies;
  index_ = (index_ + 1) % NUM_SAMPLES;

  executor->Schedule(on_update_);
}
