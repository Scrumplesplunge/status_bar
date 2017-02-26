#include "buffer.h"
#include "config.h"
#include "cpu.h"
#include "executor.h"
#include "set_status.h"
#include "uptime.h"
#include "volume.h"
#include "wall_time.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

using namespace std::chrono_literals;
using namespace std;

struct StatusBuffers {
  FixedBuffer<32> uptime;
  FixedBuffer<8> volume;
  FixedBuffer<32> wall_time;
  FixedBuffer<8> cpu_usage;

  const Buffer& Get(const string& metric_name) {
    if (metric_name == "uptime") return uptime;
    if (metric_name == "volume") return volume;
    if (metric_name == "wall_time") return wall_time;
    if (metric_name == "cpu_usage") return cpu_usage;
    throw std::runtime_error("Invalid metric name: " + metric_name);
  }
};

class UpdateStatus : public Task {
 public:
  static const size_t MAX_STATUS_LENGTH = 256;

  UpdateStatus(const Config& config, StatusBuffers* buffers)
      : format_(config.Get("status", "format")) {
    const char* i = format_.data();
    const char* end = i + format_.length();
    while (i != end) {
      // Seek through the input for the start of the first variable.
      const char* j = std::find(i, end, '$');
      buffers_.push_back(BufferView(i, j - i));
      if (j == end) break;
      // Handle the escape sequence starting at j.
      if (end - j < 2)
        throw std::runtime_error("Bad escape sequence in status string.");
      if (j[1] == '$') {
        // Sequence was an escaped '$'.
        buffers_.push_back(BufferView(j, 1));
        i = j + 2;
      } else if (j[1] == '{') {
        // Sequence is a variable reference.
        const char* k = std::find(j + 1, end, '}');
        if (k == end)
          throw std::runtime_error("Bad escape sequence in status string.");
        std::string metric_name(j + 2, k);
        const Buffer& buffer = buffers->Get(metric_name);
        buffers_.push_back(BufferView(buffer.get(), buffer.size()));
        i = k + 1;
      } else {
        // Sequence is a syntax error :(
        throw std::runtime_error("Bad escape sequence in status string.");
      }
    }
    // Verify that the format string fits into the status buffer in the worst
    // case.
    size_t length = 0;
    for (const BufferView& buffer : buffers_)
      length += buffer.size();
    if (length + 1 > MAX_STATUS_LENGTH) {
      // Concatenation of all buffers, plus a nul-terminator, would not fit
      // inside the status buffer.
      throw std::runtime_error(
          "Worst-case expansion of status string exceeds maximum supported "
          "length.");
    }
  }

 private:
  void Perform(Executor*) override {
    FixedBuffer<256> status;
    char* i = status.get();
    for (const BufferView& buffer : buffers_) {
      // Copy bytes from this buffer into the status buffer until the first '\0'
      // is found, or until the whole buffer has been copied.
      const char* buffer_begin = buffer.get();
      const char* buffer_end = buffer_begin + buffer.size();
      size_t length = std::find(buffer_begin, buffer_end, '\0') - buffer_begin;
      i = std::copy(buffer_begin, buffer_begin + length, i);
    }
    *i = '\0';
    display_.SetStatus(status.get());
  }

  std::string format_;
  std::vector<BufferView> buffers_;
  DisplayHandle display_;
};

bool FileExists(const char* filename) {
  std::ifstream file(filename);
  return file.good();
}

int main() {
  // This points to the last configuration we successfully loaded.
  Config* config = nullptr;

  std::unique_ptr<Config> default_config;
  const char* system_config_file = "/etc/status_bar.cfg";
  if (FileExists(system_config_file)) {
    default_config = std::make_unique<Config>(system_config_file, config);
    config = default_config.get();
  }

  std::unique_ptr<Config> user_config;
  const char* home_dir = getenv("HOME");
  if (home_dir != nullptr) {
    auto user_config_file = std::string(home_dir) + "/.config/status_bar.cfg";
    if (FileExists(user_config_file.c_str())) {
      user_config = std::make_unique<Config>(user_config_file, config);
      config = user_config.get();
    }
  }

  if (config == nullptr) {
    std::cerr << "No configuration file was found on the system.\n";
    return 1;
  }

  StatusBuffers buffers;
  UpdateStatus update_status(*config, &buffers);

  // System uptime.
  CalculateUptime calculate_uptime(&buffers.uptime, &update_status);
  Duration uptime_update_delay =
      config->GetAs<Duration>("uptime", "update_delay");
  PeriodicTask uptime_calculator(&calculate_uptime, uptime_update_delay.value);

  // Audio volume.
  CalculateVolume calculate_volume(*config, &buffers.volume, &update_status);
  Duration volume_update_delay =
      config->GetAs<Duration>("volume", "update_delay");
  PeriodicTask volume_calculator(&calculate_volume, volume_update_delay.value);

  // CPU usage.
  CalculateCpuUsage calculate_cpu_usage(&buffers.cpu_usage, &update_status);
  Duration cpu_usage_update_delay =
      config->GetAs<Duration>("cpu_usage", "update_delay");
  PeriodicTask cpu_usage_calculator(
      &calculate_cpu_usage, cpu_usage_update_delay.value);

  // Wall (civil) time.
  CalculateWallTime calculate_wall_time(
      *config, &buffers.wall_time, &update_status);
  Duration wall_time_update_delay =
      config->GetAs<Duration>("wall_time", "update_delay");
  PeriodicTask wall_time_calculator(
      &calculate_wall_time, wall_time_update_delay.value);

  Executor executor;
  executor.Schedule(&uptime_calculator);
  executor.Schedule(&volume_calculator);
  executor.Schedule(&wall_time_calculator);
  executor.Schedule(&cpu_usage_calculator);
  executor.Run();
}
