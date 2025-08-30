
// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(PolicyAndSafemode, DenyAndStatus){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("safemode on");
  auto r = sh.Exec("srm /x");
  ASSERT_EQ(r, "err_denied");
  sh.Exec("safemode off");
  sh.Exec("policy add deny cmd .* path .* dangerous");
  auto r2 = sh.Exec("echo a /a");
  ASSERT_EQ(r2, "err_denied");
  sh.Exec("policy clear");
}

TEST(IndexAuto, ReindexesOnWrite){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("index build /");
  sh.Exec("index auto on");
  sh.Exec("echo Hello Neon /m");
  auto res = sh.Exec("search all Hello");
  ASSERT_NE(res.find("/m"), std::string::npos);
}

TEST(Trace, OnDump){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("trace on");
  sh.Exec("echo x /t");
  ASSERT_EQ(sh.Exec("trace dump /trace.json"), "ok");
  auto out = sh.Exec("cat /trace.json");
  ASSERT_NE(out.find("\"trace\":"), std::string::npos);
}

TEST(Provenance, Show){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo data /d");
  auto p = sh.Exec("prov show /d");
  ASSERT_NE(p.find("\"cmd\":"), std::string::npos);
}

TEST(Batch, Works){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  auto r = sh.Exec("batch echo a /a ; echo b /b ; cat /a");
  ASSERT_NE(r.find("\"results\""), std::string::npos);
}

TEST(PackSign, SignsAndVerifies){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("mkdir /dir; echo one /dir/a; echo two /dir/b");
  auto b64 = sh.Exec("pack /dir");
  auto sig = sh.Exec("packsign " + b64 + " key");
  ASSERT_EQ(sh.Exec("packverify " + b64 + " " + sig + " key"), "ok");
}
