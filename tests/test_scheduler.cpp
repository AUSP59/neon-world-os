// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/scheduler.hpp"
#include <atomic>

TEST(Scheduler, SubmitRun) {
  neon::Scheduler s;
  std::atomic<int> c{0};
  s.Submit("inc", [&]{ c++; });
  s.RunOnce(std::chrono::milliseconds(5));
  EXPECT_EQ(c.load(), 1);
}
