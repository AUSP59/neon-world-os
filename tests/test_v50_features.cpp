
// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Harden, PresetShow){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  ASSERT_EQ(sh.Exec("harden preset strict"), "ok");
  auto j = sh.Exec("harden show");
  ASSERT_NE(j.find("\"wal\":"), std::string::npos);
}

TEST(Fim, BaselineAndCheck){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo a /x");
  ASSERT_EQ(sh.Exec("fim baseline base /"), "ok");
  auto rep = sh.Exec("fim check base");
  ASSERT_NE(rep.find("\"ok\":"), std::string::npos);
}

TEST(Sig, HmacVerify){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo hello /m");
  auto mac = sh.Exec("sig hmac /m key");
  ASSERT_EQ(sh.Exec("sig verify /m key " + mac), "ok");
}

TEST(Risk, Aggregates){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  auto r = sh.Exec("risk report");
  ASSERT_NE(r.find("\"dlp\":"), std::string::npos);
}
