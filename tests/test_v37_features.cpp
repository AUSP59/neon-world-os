// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(GlobXrm, Works){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo a /a.txt"); sh.Exec("echo b /b.txt"); sh.Exec("echo c /c.bin");
  auto lst = sh.Exec("glob /*.txt");
  ASSERT_NE(lst.find("/a.txt"), std::string::npos);
  auto r = sh.Exec("xrm /*.txt");
  ASSERT_NE(r.find("\"removed\":"), std::string::npos);
}

TEST(TTL, Purge){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo temp /t"); sh.Exec("ttl set /t 0");
  auto out = sh.Exec("ttl purge");
  ASSERT_NE(out.find("\"purged\":"), std::string::npos);
}

TEST(Snap, DiffList){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo a /x"); sh.Exec("snap create A");
  sh.Exec("echo b /y"); sh.Exec("snap create B");
  auto d = sh.Exec("snap difflist A B");
  ASSERT_NE(d.find("\"adds\":"), std::string::npos);
}

TEST(HashDir, Stable){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("mkdir /d; echo one /d/a; echo two /d/b");
  auto h1 = sh.Exec("hashdir /d");
  sh.Exec("append X /d/b");
  auto h2 = sh.Exec("hashdir /d");
  ASSERT_NE(h1, h2);
}

TEST(BenchFs, Runs){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  auto r = sh.Exec("benchfs 50");
  ASSERT_NE(r.find("\"ops\":"), std::string::npos);
}

TEST(CDPack, CreateRestoreStats){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("mkdir /data"); sh.Exec("echo AAAAAAAAABBBBBBBBBCCCCCCCCCDDDDDDDDD /data/f1");
  sh.Exec("echo Z"+std::string(30,'A')+"BBBBBBBBBCCCCCCCCCDDDDDDDDD /data/f2"); // shifted
  auto c = sh.Exec("cdpack create P2 /data 2048 512 8192");
  ASSERT_NE(c.find("\"files\":"), std::string::npos);
  sh.Exec("mkdir /restore");
  EXPECT_EQ(sh.Exec("cdpack restore P2 /restore"), "ok");
  auto st = sh.Exec("cdpack stats P2");
  ASSERT_NE(st.find("\"chunks\":"), std::string::npos);
}
