// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Undo, Basic){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo a /f"); sh.Exec("append b /f");
  EXPECT_EQ(sh.Exec("undo"), "{ \"on\": true, \"cap\": 16, \"depth\": 1 }");
  EXPECT_EQ(sh.Exec("undo run"), "ok");
}

TEST(Config, Load){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo 'umask: 0027\nrate.echo: 2\ndryrun: off' /cfg");
  auto r = sh.Exec("config load /cfg");
  ASSERT_NE(r.find("\"applied\":"), std::string::npos);
}

TEST(Versioned, ExportImport){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo z /a");
  auto b = sh.Exec("exportfs --versioned");
  sh.Exec("rm /a");
  EXPECT_EQ(sh.Exec("importfs " + b), "ok");
}

TEST(Macro, RecordPlay){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("macro record M1"); sh.Exec("echo x /x"); sh.Exec("macro stop");
  sh.Exec("rm /x");
  EXPECT_NE(sh.Exec("macro play M1"), "err");
}
