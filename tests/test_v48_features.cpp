
// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Help, ListsCommands){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  auto h = sh.Exec("help");
  ASSERT_NE(h.find("echo"), std::string::npos);
}

TEST(Gc, SnapWalPerf){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("snapshot save s1 /");
  sh.Exec("snapshot save s2 /");
  sh.Exec("gc snap 1");
  auto ls = sh.Exec("snapshot ls");
  ASSERT_TRUE(ls.find("s1")==std::string::npos || ls.find("s2")==std::string::npos);
  sh.Exec("wal on"); sh.Exec("echo a /x"); sh.Exec("gc wal");
  ASSERT_NE(sh.Exec("wal dump").size(), 0u);
  ASSERT_EQ(sh.Exec("gc perf"), "ok");
}

TEST(PolicyTest, Aggregates){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo policy add deny cmd echo path .* reason /p");
  sh.Exec("echo echo x /f >/pt"); // ensure file /pt with a policy case
  auto res = sh.Exec("policy test /pt");
  ASSERT_NE(res.find("\"total\":"), std::string::npos);
  sh.Exec("policy clear");
}

TEST(Conformance, Runs){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  auto r = sh.Exec("conformance");
  ASSERT_NE(r.find("\"ok\":"), std::string::npos);
}
