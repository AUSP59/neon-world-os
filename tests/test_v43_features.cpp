
// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Tags, SetGetSearch){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo data /a");
  EXPECT_EQ(sh.Exec("tag set /a owner alice"), "ok");
  ASSERT_EQ(sh.Exec("tag get /a owner"), "alice");
  auto res = sh.Exec("tag search owner ali");
  ASSERT_NE(res.find("/a"), std::string::npos);
}

TEST(PolicyEval, Deny){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("policy add deny cmd echo path .* deny-all");
  auto r = sh.Exec("policy eval echo /x");
  ASSERT_NE(r.find("\"DENY\""), std::string::npos);
  sh.Exec("policy clear");
}

TEST(ProvGraph, WritesDot){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo x /t");
  ASSERT_EQ(sh.Exec("prov graph / /prov.dot"), "ok");
  auto dot = sh.Exec("cat /prov.dot");
  ASSERT_NE(dot.find("digraph prov"), std::string::npos);
}

TEST(Perf, RecordCompare){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("perf record A 3 echo a /x");
  sh.Exec("perf record B 3 echo b /y");
  auto cmp = sh.Exec("perf compare A B");
  ASSERT_NE(cmp.find("delta_p95"), std::string::npos);
}

TEST(Hash, File){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo abc /f");
  auto h = sh.Exec("hash file /f");
  ASSERT_EQ(h.size(), 64u);
}

TEST(ReleaseCheck, Runs){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  auto rep = sh.Exec("releasecheck");
  ASSERT_NE(rep.find("\"selfcheck\":"), std::string::npos);
}
