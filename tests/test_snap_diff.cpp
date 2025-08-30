// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Snap, Diff){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo a /x");
  EXPECT_EQ(sh.Exec("snap create s1"), "ok");
  sh.Exec("echo b /y");
  EXPECT_EQ(sh.Exec("snap create s2"), "ok");
  auto js = sh.Exec("snap diff s1 s2");
  ASSERT_NE(js.find("\"added\":"), std::string::npos);
}
