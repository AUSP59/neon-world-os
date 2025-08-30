// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Cache, StatsAndEffect){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo hello /a");
  sh.Exec("cache clear");
  auto s1 = sh.Exec("cat /a"); // miss
  auto s2 = sh.Exec("cat /a"); // hit
  auto st = sh.Exec("cache stats");
  ASSERT_NE(st.find("\"hits\":"), std::string::npos);
}

TEST(Metrics, PercentilesAndHealth){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  for (int i=0;i<10;i++) sh.Exec("echo x /x");
  auto m = sh.Exec("metrics pct echo");
  ASSERT_NE(m.find("\"p50\":"), std::string::npos);
  auto h = sh.Exec("health");
  ASSERT_NE(h.find("\"error_rate\":"), std::string::npos);
}

TEST(Dpack, CreateRestoreStats){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("mkdir /d; echo aaaabbbbccccdddd /d/a; echo 1111222233334444 /d/b");
  auto out = sh.Exec("dpack create P1 /d 4");
  ASSERT_NE(out.find("\"files\":"), std::string::npos);
  sh.Exec("mkdir /restore");
  EXPECT_EQ(sh.Exec("dpack restore P1 /restore"), "ok");
  auto st = sh.Exec("dpack stats P1");
  ASSERT_NE(st.find("\"chunks\":"), std::string::npos);
}

TEST(Scan, SecretsAndPII){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo ghp_123456789012345678901234567890123456 /s1");
  sh.Exec("echo user@example.com /s2");
  auto a = sh.Exec("scan secrets /");
  auto b = sh.Exec("scan pii /");
  ASSERT_NE(a.find("\"github_token\""), std::string::npos);
  ASSERT_NE(b.find("\"email\""), std::string::npos);
}

TEST(Trash, PurgeOlderThan){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo q /q");
  auto id = sh.Exec("srm /q");
  ASSERT_NE(id, "err");
  auto ok = sh.Exec("trash purge older-than 0");
  ASSERT_EQ(ok, "ok");
}
