#pragma once

#include "buffer.h"
#include "executor.h"

class CalculateCpuUsage : public Task {
 public:
  CalculateCpuUsage(Buffer* buffer, Task* on_update);
  void Perform(Executor* executor) override;

 private:
  Buffer* buffer_;
  Task* on_update_;

  static const int NUM_SAMPLES = 10;
  int index_ = 0;
  int64_t busy_jiffies_[NUM_SAMPLES] = {0};
  int64_t total_jiffies_[NUM_SAMPLES] = {0};
};
