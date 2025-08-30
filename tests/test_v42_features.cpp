
// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(SearchFuzzy, FindsNearMatches){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo Hello Neon /a");
  sh.Exec("index build /");
  auto out = sh.Exec("search fuzzy Neen 2"); // fuzzy for "Neon"
  ASSERT_NE(out.find("/a"), std::string::npos);
}

TEST(SearchRank, OrdersByScore){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo neon neon world /x");
  sh.Exec("echo neon docs /y");
  sh.Exec("index build /");
  auto out = sh.Exec("search rank any neon world");
  // should list /x before /y typically
  ASSERT_LT(out.find("/x"), out.find("/y"));
}

TEST(Quarantine, RoundTrip){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo secret /s");
  auto id = sh.Exec("quarantine add /s");
  ASSERT_NE(id, "err");
  ASSERT_EQ(sh.Exec("cat /s"), "err"); // removed
  ASSERT_EQ(sh.Exec("quarantine restore " + id), "ok");
  ASSERT_EQ(sh.Exec("cat /s"), "secret");
}

TEST(Shred, Overwrites){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo 0123456789 /t");
  sh.Exec("shred /t 1");
  auto out = sh.Exec("cat /t");
  ASSERT_NE(out, "0123456789");
  ASSERT_EQ(out.size(), std::string(\"0123456789\").size());
}

TEST(Util, SizeAndDuration){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  ASSERT_EQ(sh.Exec("util size 1MiB"), "1048576");
  ASSERT_EQ(sh.Exec("util duration 5m"), "300");
}

TEST(LintInclusive, DetectsTerms){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo use whitelist and master terms /l");
  auto rep = sh.Exec("lint inclusive /");
  ASSERT_NE(rep.find("\"suggest\": \"allowlist\""), std::string::npos);
}

TEST(Selfcheck, JSONish){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  auto j = sh.Exec("selfcheck");
  ASSERT_NE(j.find("\"verify\":"), std::string::npos);
}
