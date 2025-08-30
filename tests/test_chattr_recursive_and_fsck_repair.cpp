// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Chattr, Recursive){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("mkdir /d"); sh.Exec("echo a /d/a"); sh.Exec("echo b /d/b");
  EXPECT_EQ(sh.Exec("chattr -R +i /d"), "ok");
  EXPECT_EQ(sh.Exec("append Z /d/a"), "err");
  EXPECT_EQ(sh.Exec("chattr -R -i /d"), "ok");
}

TEST(Fsck, Repair){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo a /k"); sh.Exec("rm /k");
  auto out = sh.Exec("fsck repair");
  ASSERT_NE(out.find("\"pruned\":"), std::string::npos);
}
