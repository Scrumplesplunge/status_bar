#pragma once

#include "buffer.h"
#include "executor.h"

class CalculateUptime : public Task {
 public:
  CalculateUptime(Buffer* buffer, Task* on_update);

 private:
  void Perform(Executor* executor) override;

  Buffer* buffer_;
  Task* on_update_;
};
