#pragma once

#include <chrono>
#include <queue>
#include <vector>

class Executor;

// A task is an action that we would like to perform.
class Task {
 public:
  // Perform the task. The executor which is performing the task is passed into
  // the task. This allows this task to schedule further tasks.
  virtual void Perform(Executor* executor) = 0;
};

// A task which we would like to perform periodically.
class PeriodicTask : public Task {
 public:
  PeriodicTask(Task* underlying_task, std::chrono::milliseconds delay);
  void Perform(Executor* executor) override;

 private:
  Task* underlying_task_;
  std::chrono::milliseconds delay_;
};

class Executor {
 public:
  using time_point = std::chrono::steady_clock::time_point;
  static time_point now() { return std::chrono::steady_clock::now(); }

  // Schedule a new task to be performed. Optionally specify a time when it
  // should run.
  void Schedule(Task* task, time_point time = now());
  
  // Repeatedly perform tasks until the task queue is empty.
  void Run();

 private:
  struct TaskEntry {
    bool operator>(const TaskEntry& other) const;

    Task* task;
    time_point scheduled_time;
  };

  using SchedulerContainer = std::vector<TaskEntry>;
  using Comparator = std::greater<TaskEntry>;  // Soonest time at the top.
  using SchedulerQueue =
      std::priority_queue<TaskEntry, SchedulerContainer, Comparator>;

  SchedulerQueue queue_;
};
