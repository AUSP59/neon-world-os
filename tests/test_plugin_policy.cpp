// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Plugin, PolicyRejectsRelativePath){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  auto r = sh.Exec("plugin load ./not-allowed.so");
  EXPECT_EQ(r, "err");
}
