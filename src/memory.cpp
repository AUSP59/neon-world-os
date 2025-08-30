// SPDX-License-Identifier: MIT
#include "neon/memory.hpp"
#include <new>
#include <stdexcept>

namespace neon {

MemoryManager::MemoryManager(std::size_t capacity_bytes)
  : capacity_(capacity_bytes) {}

void* MemoryManager::Allocate(std::size_t n) {
  std::lock_guard<std::mutex> lk(mu_);
  std::size_t used = 0;
  for (auto& kv : allocs_) used += kv.second;
  if (used + n > capacity_) throw std::bad_alloc();
  void* p = ::operator new(n);
  allocs_[p] = n;
  return p;
}

void MemoryManager::Free(void* p) {
  if (!p) return;
  std::lock_guard<std::mutex> lk(mu_);
  allocs_.erase(p);
  ::operator delete(p);
}

std::size_t MemoryManager::Used() const {
  std::lock_guard<std::mutex> lk(mu_);
  std::size_t used = 0;
  for (auto& kv : allocs_) used += kv.second;
  return used;
}

}  // namespace neon
