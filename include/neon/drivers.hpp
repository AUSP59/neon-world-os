// SPDX-License-Identifier: MIT
#pragma once
#include <chrono>
#include <random>

namespace neon {

inline std::uint64_t NowMillis() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

class FastRng {
 public:
  FastRng() : rng_(std::random_device{}()) {}
  uint32_t Next() { return dist_(rng_); }
 private:
  std::mt19937 rng_;
  std::uniform_int_distribution<uint32_t> dist_;
};

} // namespace neon
