#pragma once

#include "buffer.h"
#include "config.h"
#include "executor.h"

class CalculateWallTime : public Task {
 public:
  CalculateWallTime(const Config& config, Buffer* buffer, Task* on_update);

 private:
  void Perform(Executor* executor) override;

  Buffer* buffer_;
  Task* on_update_;
  const std::string format_;
};
