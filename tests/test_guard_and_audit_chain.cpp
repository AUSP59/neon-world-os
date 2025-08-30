// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(GuardAudit, RollbackAndVerify){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  auto r = sh.Exec("guard rm /nope");
  (void)r;
  sh.Exec("audit on audit.jsonl");
  sh.Exec("audit mode json");
  sh.Exec("audit key secret");
  sh.Exec("echo x /a");
  sh.Exec("audit off");
  EXPECT_EQ(sh.Exec("audit verify audit.jsonl"), "ok");
}
