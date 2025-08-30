// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"
TEST(Shell, Policy){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  EXPECT_EQ(sh.Exec("policy on"), "on");
  EXPECT_EQ(sh.Exec("mkdir /x"), "err"); // denied by policy
  EXPECT_EQ(sh.Exec("policy allow mkdir"), "ok");
  EXPECT_EQ(sh.Exec("mkdir /x"), "ok"); // now allowed
}
