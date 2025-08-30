
// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(WAL, RecordsAndReplays){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("wal on");
  sh.Exec("echo a /x");
  sh.Exec("append b /x");
  auto dump = sh.Exec("wal dump");
  ASSERT_NE(dump.find("append b /x"), std::string::npos);
  sh.Exec("wal off");
  sh.Exec("wal replay");
  ASSERT_EQ(sh.Exec("cat /x"), "ab");
}

TEST(Snapshot, SaveRestore){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo v1 /f");
  sh.Exec("snapshot save s1 /");
  sh.Exec("echo v2 /f");
  sh.Exec("snapshot restore s1");
  ASSERT_EQ(sh.Exec("cat /f"), "v1");
}

TEST(Metrics, OnDump){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("metrics on");
  sh.Exec("echo x /m");
  auto rep = sh.Exec("metrics dump");
  ASSERT_NE(rep.find("\"commands\":"), std::string::npos);
}

TEST(OSSAudit, PresenceAndSPDX){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  auto j = sh.Exec("oss audit");
  ASSERT_NE(j.find("README.md"), std::string::npos);
}

TEST(Manifest, Writes){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  ASSERT_EQ(sh.Exec("manifest /MANIFEST.txt"), "ok");
  auto m = sh.Exec("cat /MANIFEST.txt");
  ASSERT_NE(m.size(), 0u);
}
