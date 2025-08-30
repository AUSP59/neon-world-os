// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Shell, BenchAndPolicySaveLoad){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  auto js = sh.Exec("bench 3 echo ok /a");
  ASSERT_NE(js.find("\"iters\":"), std::string::npos);
  sh.Exec("policy on"); sh.Exec("policy allow mkdir");
  ASSERT_EQ(sh.Exec("policy save /p.txt"), "ok");
  sh.Exec("policy off"); // turn off and clear by loading
  ASSERT_EQ(sh.Exec("policy load /p.txt"), "ok");
  auto st = sh.Exec("policy status");
  ASSERT_NE(st.find("on"), std::string::npos);
}
