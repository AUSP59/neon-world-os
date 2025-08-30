// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Shell, FindGlob){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("mkdir /d"); sh.Exec("echo one /d/a.txt"); sh.Exec("echo two /d/b.log");
  auto out = sh.Exec("find /d *.txt");
  ASSERT_NE(out.find("/d/a.txt"), std::string::npos);
  ASSERT_EQ(out.find("/d/b.log"), std::string::npos);
}
