// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Meta, ChmodUtimeStat){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo abc /a");
  EXPECT_EQ(sh.Exec("chmod /a 0640"), "ok");
  EXPECT_EQ(sh.Exec("utime /a 1700000000"), "ok");
  auto st = sh.Exec("stat /a");
  ASSERT_NE(st.find("\"size\":"), std::string::npos);
}

TEST(IO, AppendTruncateReadRange){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo hi /x");
  EXPECT_EQ(sh.Exec("append test /x"), "ok");
  EXPECT_EQ(sh.Exec("truncate /x 2"), "ok");
  auto rr = sh.Exec("catrange /x 0 2");
  ASSERT_EQ(rr, "hi");
}
