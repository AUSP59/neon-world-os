// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Misc, AliasTreeHash){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("alias mk echo");
  EXPECT_EQ(sh.Exec("mk hi /a"), "ok");
  auto tr = sh.Exec("tree /");
  ASSERT_NE(tr.find("a"), std::string::npos);
  auto h = sh.Exec("sha256 /a");
  ASSERT_EQ(h.size(), 64u);
}

TEST(Misc, LineEndingsUtf8){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo a\\r\\nb /in");
  EXPECT_EQ(sh.Exec("dos2unix /in /out"), "ok");
  EXPECT_EQ(sh.Exec("unix2dos /out /out2"), "ok");
  auto u = sh.Exec("utf8check /out");
  ASSERT_NE(u.find("\"ok\":"), std::string::npos);
}
