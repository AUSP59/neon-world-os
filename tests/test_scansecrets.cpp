// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Scan, Secrets){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("mkdir /sec"); sh.Exec("echo AKIA1234567890ABCD /sec/a");
  auto out = sh.Exec("scansecrets /sec");
  ASSERT_NE(out.find("\"total\":"), std::string::npos);
}
