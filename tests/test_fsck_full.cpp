// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Shell, FsckFull){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo z /a");
  auto out = sh.Exec("fsck full");
  ASSERT_NE(out.find("\"merkleroot\""), std::string::npos);
}
