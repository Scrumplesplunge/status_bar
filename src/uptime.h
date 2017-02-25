#pragma once

#include "executor.h"

class CalculateUptime : public Task {
 public:
  CalculateUptime(char* buffer, size_t size);

  void Perform(Executor* executor) override;

 private:
  char* buffer_;
  size_t size_;
};
