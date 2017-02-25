#pragma once

#include "buffer.h"
#include "executor.h"

class CalculateUptime : public Task {
 public:
  CalculateUptime(Buffer* buffer, Task* on_update);
  void Perform(Executor* executor) override;

 private:
  Buffer* buffer_;
  Task* on_update_;
};
