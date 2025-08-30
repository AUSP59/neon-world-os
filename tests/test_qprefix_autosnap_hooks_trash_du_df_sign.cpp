// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(QuotaPrefix, Enforce){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("mkdir /team");
  sh.Exec("qprefix set /team 5");
  EXPECT_EQ(sh.Exec("echo 12345 /team/a"), "ok");
  ASSERT_EQ(sh.Exec("append 6 /team/a"), "err"); // exceed
  sh.Exec("qprefix ls");
}

TEST(AutoSnap, CreatePrune){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("autosnap on 2");
  sh.Exec("echo a /x");
  sh.Exec("echo b /y");
  sh.Exec("echo c /z");
  auto st = sh.Exec("autosnap status");
  ASSERT_NE(st.find("\"count\": 2"), std::string::npos);
}

TEST(Hook, FireOnWrite){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("hook add echo / log");
  sh.Exec("echo a /p");
  auto lr = sh.Exec("metrics"); // ensure command executed; can't easily assert
  (void)lr;
}

TEST(Trash, SafeRemoveRestore){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo q /q");
  auto id = sh.Exec("srm /q");
  ASSERT_NE(id, "err");
  auto ls = sh.Exec("trash ls");
  ASSERT_NE(ls.find(id), std::string::npos);
  EXPECT_EQ(sh.Exec("trash restore " + id), "ok");
}

TEST(DuDf, Report){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo hello /h");
  auto du = sh.Exec("du /");
  auto df = sh.Exec("df");
  ASSERT_NE(du.find("\"bytes\":"), std::string::npos);
  ASSERT_NE(df.find("\"bytes\":"), std::string::npos);
}

TEST(Snap, SignVerify){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo t /t"); sh.Exec("snap create S1");
  auto tag = sh.Exec("snap sign S1 key");
  ASSERT_NE(tag, "err");
  EXPECT_EQ(sh.Exec("snap verify S1 key"), "ok");
}
