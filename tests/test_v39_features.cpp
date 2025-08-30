// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Versions, CaptureListRestore){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo a /f"); sh.Exec("append b /f");
  auto lst = sh.Exec("ver ls /f");
  ASSERT_NE(lst.size(), 0u);
  // Take newest ts
  std::istringstream in(lst); std::string line, ts;
  while (std::getline(in, line)){ if (line.size()){ ts = line.substr(0, line.find('\t')); break; } }
  ASSERT_NE(ts, "");
  EXPECT_EQ(sh.Exec("ver restore /f " + ts), "ok");
}

TEST(CAS, PutGetLinkRestoreStat){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo data /d"); auto sha = sh.Exec("cas put /d");
  ASSERT_NE(sha, "err");
  EXPECT_EQ(sh.Exec("cas get " + sha + " /e"), "ok");
  auto sha2 = sh.Exec("cas link /e");
  ASSERT_EQ(sha2, sha);
  EXPECT_EQ(sh.Exec("cas restore /e"), "ok");
  auto st = sh.Exec("cas stat");
  ASSERT_NE(st.find("\"files\":"), std::string::npos);
}

TEST(Attest, MakeVerify){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("mkdir /dir; echo one /dir/a; echo two /dir/b");
  auto b64 = sh.Exec("attest make /dir key");
  ASSERT_NE(b64, "err");
  EXPECT_EQ(sh.Exec("attest verify " + b64 + " key"), "ok");
}

TEST(NsQuota, SetLs){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("ns create team1");
  EXPECT_EQ(sh.Exec("nsquota set team1 100"), "ok");
  auto rep = sh.Exec("nsquota ls");
  ASSERT_NE(rep.find("/@ns/"), std::string::npos);
}

TEST(WriteAtom, Ok){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  EXPECT_EQ(sh.Exec("writeatom /x DATA"), "ok");
  ASSERT_EQ(sh.Exec("cat /x"), "DATA");
}

TEST(Mask, EmailsDigits){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo contact me at user@example.com 1234 /m");
  EXPECT_EQ(sh.Exec("mask emails /m"), "ok");
  EXPECT_EQ(sh.Exec("mask digits /m"), "ok");
  auto out = sh.Exec("cat /m");
  ASSERT_NE(out.find("***@***"), std::string::npos);
  ASSERT_EQ(out.find("1"), std::string::npos);
}
