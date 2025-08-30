// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Locks, ExclusiveTtl){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo a /f");
  EXPECT_EQ(sh.Exec("lock set /f 10000"), "ok");
  EXPECT_EQ(sh.Exec("append Z /f"), "err");
  EXPECT_EQ(sh.Exec("lock release /f"), "ok");
}

TEST(Index, BuildSearch){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo Hello World /a");
  sh.Exec("echo World of Neon /b");
  auto st = sh.Exec("index build /");
  auto out = sh.Exec("index search World");
  ASSERT_NE(out.find("/a"), std::string::npos);
  ASSERT_NE(out.find("/b"), std::string::npos);
}

TEST(Snap, Diff){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo a /x"); sh.Exec("snap create A");
  sh.Exec("echo b /y"); sh.Exec("snap create B");
  auto d = sh.Exec("snap diff A B");
  ASSERT_NE(d.find("\"adds\""), std::string::npos);
}

TEST(Seal, Verify){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo secret /s");
  auto h = sh.Exec("seal /s");
  ASSERT_EQ(sh.Exec("verifyseal /s"), "ok");
}
