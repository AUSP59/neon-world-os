// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <cstdint>
#include <cstddef>
#include "neon/shell.hpp"
#include "neon/scheduler.hpp"
#include "neon/vfs.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size){
  static neon::Scheduler s; static neon::Vfs v; static neon::Shell sh(s, v);
  if (size == 0 || size > 4096) return 0;
  std::string in(reinterpret_cast<const char*>(data), size);
  (void)sh.Exec(in);
  return 0;
}
