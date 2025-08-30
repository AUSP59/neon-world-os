// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"
#include <chrono>

TEST(Shell, Sleep){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  auto t0 = std::chrono::steady_clock::now();
  sh.Exec("sleep 5");
  auto t1 = std::chrono::steady_clock::now();
  auto us = std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();
  ASSERT_GT(us, 1000); // at least 1ms elapsed
}
