// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Metrics, Histogram){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  for (int i=0;i<5;i++) sh.Exec("echo x /h");
  auto m = sh.Exec("metrics hist echo 5");
  ASSERT_NE(m.find("\"hist\""), std::string::npos);
}
