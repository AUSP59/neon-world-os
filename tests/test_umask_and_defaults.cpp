// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Perms, UmaskAndDefaults){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("umask 0027"); // more restrictive
  sh.Exec("echo data /a");
  auto st = sh.Exec("stat /a");
  ASSERT_NE(st.find("\"size\":"), std::string::npos);
}
