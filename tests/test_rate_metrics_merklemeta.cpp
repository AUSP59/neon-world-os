// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Shell, RateAndMetricsAndMerkleMeta){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("rate set echo 1");
  auto r1 = sh.Exec("echo a /a"); (void)r1;
  auto r2 = sh.Exec("echo b /b"); // likely err due to rate 1/min
  (void)r2;
  auto m = sh.Exec("metrics");
  ASSERT_NE(m.find("\"total\":"), std::string::npos);
  sh.Exec("mkdir /d"); sh.Exec("echo x /d/a");
  auto m1 = sh.Exec("merkleroot /d");
  sh.Exec("utime /d/a 1700000001"); // change meta
  auto m2 = sh.Exec("merkleroot /d --meta");
  ASSERT_NE(m1, m2);
}
