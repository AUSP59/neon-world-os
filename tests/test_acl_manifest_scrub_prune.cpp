// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(ACL, EnforceRoles){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("acl create dev"); sh.Exec("acl grant dev R /");
  sh.Exec("acl enforce on");
  EXPECT_EQ(sh.Exec("as dev echo x /a"), "err");
  sh.Exec("acl grant dev W /a");
  EXPECT_NE(sh.Exec("as dev echo x /a"), "err");
}

TEST(Manifest, MakeVerify){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo one /A"); sh.Exec("echo two /B");
  auto m = sh.Exec("manifest make /");
  auto v1 = sh.Exec("manifest verify " + m);
  ASSERT_NE(v1.find("\"ok\":"), std::string::npos);
  sh.Exec("append Z /A");
  auto v2 = sh.Exec("manifest verify " + m);
  ASSERT_NE(v2.find("\"mismatch\":"), std::string::npos);
}

TEST(Scrub, Sealed){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo sss /s"); auto h = sh.Exec("seal /s");
  sh.Exec("append X /s");
  auto out = sh.Exec("scrub /");
  ASSERT_NE(out.find("\"sealed_bad\":"), std::string::npos);
}

TEST(Snap, PruneKeep){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo a /x"); sh.Exec("snap create S1");
  sh.Exec("echo b /y"); sh.Exec("snap create S2");
  sh.Exec("echo c /z"); sh.Exec("snap create S3");
  EXPECT_EQ(sh.Exec("snap prune keep 1"), "ok");
}
