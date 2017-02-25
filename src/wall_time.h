#pragma once

#include "buffer.h"
#include "executor.h"

class CalculateWallTime : public Task {
 public:
  CalculateWallTime(Buffer* buffer, Task* on_update);
  void Perform(Executor* executor) override;

 private:
  Buffer* buffer_;
  Task* on_update_;
};
