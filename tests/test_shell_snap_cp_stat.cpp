// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Shell, SnapAndDoctor){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("mkdir /d");
  sh.Exec("echo hi /d/a.txt");
  EXPECT_EQ(sh.Exec("snap create s1"), "ok");
  sh.Exec("rm -r /d");
  EXPECT_EQ(sh.Exec("snap restore s1"), "ok");
  auto st = sh.Exec("stat /d/a.txt");
  ASSERT_NE(st.find("\"is_dir\":false"), std::string::npos);
  auto doc = sh.Exec("doctor");
  ASSERT_NE(doc.find("\"version\""), std::string::npos);
}
