
// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Dlp, BlocksSensitiveOps){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo user@example.com /m");
  sh.Exec("dlp on");
  auto r = sh.Exec("pack /");
  ASSERT_EQ(r, "err_dlp");
  sh.Exec("dlp off");
}

TEST(Tx, BeginRollback){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo v1 /f");
  sh.Exec("tx begin t1 /");
  sh.Exec("echo v2 /f");
  sh.Exec("tx rollback t1");
  ASSERT_EQ(sh.Exec("cat /f"), "v1");
}

TEST(AuditExport, ProducesTimeline){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("wal on");
  sh.Exec("echo a /x");
  sh.Exec("watch add w1 cmd .* path /x");
  sh.Exec("append b /x");
  auto out = sh.Exec("audit export /audit.ndjson");
  auto c = sh.Exec("cat /audit.ndjson");
  ASSERT_NE(c.size(), 0u);
}

TEST(ClassifyAndAnonymize, TagsAndRedacts){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo token=ABCDEF123456 /s");
  sh.Exec("redact add [A-Z0-9]{6,}");
  sh.Exec("classify /");
  auto n = sh.Exec("anonymize /");
  ASSERT_NE(n, "0");
}
