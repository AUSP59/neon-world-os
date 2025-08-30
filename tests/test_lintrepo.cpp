// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Lint, RepoDocs){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  auto lr = sh.Exec("lintrepo");
  ASSERT_NE(lr.find("\"ok\":"), std::string::npos);
}
