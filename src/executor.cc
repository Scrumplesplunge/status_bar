#include "executor.h"

#include <thread>

PeriodicTask::PeriodicTask(
    Task* underlying_task, std::chrono::milliseconds delay)
    : underlying_task_(underlying_task), delay_(delay) {}

void PeriodicTask::Perform(Executor* executor) {
  underlying_task_->Perform(executor);
  executor->Schedule(this, Executor::now() + delay_);
}

void Executor::Schedule(Task* task, Executor::time_point time) {
  queue_.push(TaskEntry{task, time});
}

void Executor::Run() {
  while (queue_.size() > 0) {
    // Remove the task which needs to be performed.
    TaskEntry entry = queue_.top();
    queue_.pop();
    // Wait until this task needs to be performed, and then perform it.
    std::this_thread::sleep_until(entry.scheduled_time);
    entry.task->Perform(this);
  }
}

bool Executor::TaskEntry::operator>(const Executor::TaskEntry& other) const {
  // Compare task entries only by the time when they are scheduled to run.
  return scheduled_time > other.scheduled_time;
}
