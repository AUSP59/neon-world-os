// SPDX-License-Identifier: MIT
#pragma once
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <unordered_map>

namespace neon {

class MemoryManager {
 public:
  explicit MemoryManager(std::size_t capacity_bytes);
  void* Allocate(std::size_t n);
  void  Free(void* p);
  std::size_t Used() const;
  std::size_t Capacity() const { return capacity_; }

 private:
  std::size_t capacity_;
  mutable std::mutex mu_;
  std::unordered_map<void*, std::size_t> allocs_;
};

}  // namespace neon
