// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Shell, HelpAndEcho) {
  neon::Scheduler sched;
  neon::Vfs vfs;
  neon::Shell sh(sched, vfs);
  EXPECT_NE(sh.Exec("help").find("Commands:"), std::string::npos);
  EXPECT_EQ(sh.Exec("echo hello /a.txt"), "ok");
  EXPECT_EQ(sh.Exec("cat /a.txt"), "hello");
}
