#pragma once

#include "buffer.h"
#include "config.h"
#include "executor.h"

class CalculateWallTime : public Task {
 public:
  CalculateWallTime(const Section& config, Buffer* buffer, Task* on_update);
  void Perform(Executor* executor) override;

 private:
  Buffer* buffer_;
  Task* on_update_;
  const std::string format_;
};
