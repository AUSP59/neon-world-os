// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Tx, BeginCommitAbort){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo a /f");
  EXPECT_EQ(sh.Exec("tx begin"), "ok");
  sh.Exec("append b /f");
  EXPECT_EQ(sh.Exec("tx abort"), "ok");
  auto out = sh.Exec("cat /f");
  ASSERT_EQ(out, "a");
  sh.Exec("tx begin"); sh.Exec("append Z /f"); EXPECT_EQ(sh.Exec("tx commit"), "ok");
}

TEST(NS, CreateSwitchWrap){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("ns create team1"); sh.Exec("ns switch team1");
  EXPECT_NE(sh.Exec("ns who"), "");
  auto o = sh.Exec("nswrap team1 echo data /x");
  ASSERT_EQ(sh.Exec("cat /@ns/team1/x"), "data");
}

TEST(Search, AllAnyPhrase){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo Hello World /a"); sh.Exec("echo World of Neon /b");
  sh.Exec("index build /");
  auto all = sh.Exec("search all World Neon");
  ASSERT_NE(all.find("/b"), std::string::npos);
  auto any = sh.Exec("search any Hello Foo");
  ASSERT_NE(any.find("/a"), std::string::npos);
  auto ph = sh.Exec("search phrase World of Neon");
  ASSERT_NE(ph.find("/b"), std::string::npos);
}

TEST(B64, EncDec){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo xyz /t");
  sh.Exec("b64enc /t /t.b64");
  sh.Exec("b64dec /t.b64 /u");
  ASSERT_EQ(sh.Exec("cat /u"), "xyz");
}

TEST(SplitJoin, Works){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo 0123456789 /big");
  auto r = sh.Exec("split /big 3 /part");
  ASSERT_NE(r.find("\"parts\":"), std::string::npos);
  EXPECT_EQ(sh.Exec("join /part /out"), "ok");
}

TEST(Plan, Reports){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  auto p = sh.Exec("plan echo /a");
  ASSERT_NE(p.find("\"perm\":"), std::string::npos);
}
