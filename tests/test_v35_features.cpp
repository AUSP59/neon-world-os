// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(V35, DiffDirAndMerkle){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("mkdir /A"); sh.Exec("mkdir /B");
  sh.Exec("echo x /A/f"); sh.Exec("echo x /B/f");
  auto d0 = sh.Exec("diffdir /A /B");
  ASSERT_NE(d0.find("\"adds\": 0"), std::string::npos);
  sh.Exec("append y /B/f");
  auto d1 = sh.Exec("diffdir /A /B");
  ASSERT_NE(d1.find("\"mods\": 1"), std::string::npos);
  auto mt = sh.Exec("merkletree /B/f 2");
  ASSERT_NE(mt.find("\"root\":"), std::string::npos);
}

TEST(V35, FindTemplateChmodSealall){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo hello /t.txt");
  sh.Exec("set NAME world");
  sh.Exec("template /t.txt /out.txt");
  auto g = sh.Exec("grep world /");
  ASSERT_NE(g.find("/out.txt"), std::string::npos);
  sh.Exec("sealall /");
  auto scb = sh.Exec("scrub /");
  ASSERT_NE(scb.find("\"sealed_ok\":"), std::string::npos);
}

TEST(V35, PackUnpackTimeRgrep){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("mkdir /dir"); sh.Exec("echo foo /dir/a"); sh.Exec("echo bar /dir/b");
  auto b64 = sh.Exec("pack /dir");
  sh.Exec("mkdir /dir2"); sh.Exec("unpack " + b64 + " /");
  auto out = sh.Exec("ls /dir");
  ASSERT_NE(out.find("a"), std::string::npos);
  auto t = sh.Exec("time echo z /z");
  ASSERT_NE(t.find("\"us\":"), std::string::npos);
  auto rg = sh.Exec("rgrep / foo");
  ASSERT_NE(rg.find("/dir/a"), std::string::npos);
}
