// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Immutability, Chattr){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo a /f");
  EXPECT_EQ(sh.Exec("chattr +i /f"), "ok");
  auto r = sh.Exec("append Z /f");
  ASSERT_EQ(r, "err");
  EXPECT_EQ(sh.Exec("lsattr /f"), "i");
  EXPECT_EQ(sh.Exec("chattr -i /f"), "ok");
}

TEST(Vars, SetExpand){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("set NAME world");
  EXPECT_EQ(sh.Exec("echo hello_${NAME} /a"), "ok");
  auto wc = sh.Exec("wc /a");
  ASSERT_NE(wc.find("\"bytes\":"), std::string::npos);
}

TEST(Diff, FileDiff){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo one\\nX\\nthree /A");
  sh.Exec("echo one\\ntwo\\nthree /B");
  auto d = sh.Exec("difffile /A /B");
  ASSERT_NE(d.find('+'), std::string::npos);
  ASSERT_NE(d.find('-'), std::string::npos);
}

TEST(Regex, Replace){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo foo bar foo /t");
  EXPECT_EQ(sh.Exec("repl foo baz /t"), "ok");
  auto g = sh.Exec("grep baz /");
  ASSERT_NE(g.find("/t"), std::string::npos);
}

TEST(Snapshots, TagList){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo v /x");
  EXPECT_EQ(sh.Exec("snap create S1"), "ok");
  EXPECT_EQ(sh.Exec("snap tag S1 desc"), "ok");
  auto ls = sh.Exec("snap ls");
  ASSERT_NE(ls.find("S1"), std::string::npos);
}
