// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"
TEST(Shell, SchemaPs){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  auto out = sh.Exec("schema ps");
  ASSERT_NE(out, "err");
  ASSERT_NE(out.find("\"title\":\"neon.ps\""), std::string::npos);
}
