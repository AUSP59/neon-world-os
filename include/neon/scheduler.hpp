// SPDX-License-Identifier: MIT
#pragma once
#include "neon/export.hpp"
#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <thread>
#include <vector>

namespace neon {

using TaskFn = std::function<void()>;

struct Task {
  std::uint64_t id;
  std::string name;
  TaskFn fn;
};

class NEON_API Scheduler {
 public:
  Scheduler();
  ~Scheduler();
  std::uint64_t Submit(std::string name, TaskFn fn);
  void RunOnce(std::chrono::milliseconds budget = std::chrono::milliseconds(5));
  void Stop();
  std::vector<Task> Snapshot() const;

 private:
  mutable std::mutex mu_;
  std::queue<Task> q_;
  std::atomic<std::uint64_t> next_id_{1};
  bool stopping_ = false;
};

}  // namespace neon
