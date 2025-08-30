// SPDX-License-Identifier: MIT
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"
#include <cstdint>
#include <cstddef>
#include <string>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  neon::Scheduler sched;
  neon::Vfs vfs;
  neon::Shell sh(sched, vfs);
  std::string s(reinterpret_cast<const char*>(data), size);
  (void)sh.Exec(s);
  return 0;
}
