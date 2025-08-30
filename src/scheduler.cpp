// SPDX-License-Identifier: MIT
#include "neon/scheduler.hpp"

namespace neon {

Scheduler::Scheduler() = default;
Scheduler::~Scheduler() = default;

std::uint64_t Scheduler::Submit(std::string name, TaskFn fn) {
  std::lock_guard<std::mutex> lk(mu_);
  const auto id = next_id_++;
  q_.push(Task{ id, std::move(name), std::move(fn) });
  return id;
}

void Scheduler::RunOnce(std::chrono::milliseconds budget) {
  const auto until = std::chrono::steady_clock::now() + budget;
  while (std::chrono::steady_clock::now() < until) {
    Task cur;
    {
      std::lock_guard<std::mutex> lk(mu_);
      if (stopping_ || q_.empty()) break;
      cur = q_.front(); q_.pop();
    }
    if (cur.fn) cur.fn();
  }
}

void Scheduler::Stop() {
  std::lock_guard<std::mutex> lk(mu_);
  stopping_ = true;
  while (!q_.empty()) q_.pop();
}

std::vector<Task> Scheduler::Snapshot() const {
  std::lock_guard<std::mutex> lk(mu_);
  std::queue<Task> tmp = q_;
  std::vector<Task> out;
  while (!tmp.empty()) { out.push_back(tmp.front()); tmp.pop(); }
  return out;
}

}  // namespace neon
