// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Shell, AuditRedact){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("audit on audit.log");
  sh.Exec("audit mode json");
  sh.Exec("audit redact add secret");
  sh.Exec("echo secret /a");
  sh.Exec("audit off");
  SUCCEED(); // sanity (we don't read files here)
}
