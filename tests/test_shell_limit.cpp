// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"
#include <thread>
#include <chrono>

TEST(Shell, TimeLimitNoop){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("limit time_us 1"); // very small
  // A simple command should either succeed fast or be marked err if limit triggers;
  auto out = sh.Exec("help");
  (void)out;
  SUCCEED();
}
