
// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Vars, Expand){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("var set NAME world");
  auto out = sh.Exec("echo Hello ${NAME} /t");
  ASSERT_EQ(sh.Exec("cat /t"), "Hello world");
}

TEST(Hold, PreventsDelete){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo data /d");
  sh.Exec("hold set /d 60");
  auto r = sh.Exec("rm /d");
  ASSERT_EQ(r, "err_hold");
  sh.Exec("hold clear /d");
  ASSERT_EQ(sh.Exec("rm /d"), "ok");
}

TEST(Scan, SecretsAndPII){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo Contact me at user@example.com /m");
  auto s1 = sh.Exec("scan secrets /");
  auto s2 = sh.Exec("scan pii /");
  ASSERT_NE(s1.find("email"), std::string::npos);
  ASSERT_NE(s2.find("email"), std::string::npos);
}

TEST(Redact, AddApply){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo token=ABCDEF123456 /s");
  sh.Exec("redact add [A-Z0-9]{6,}");
  sh.Exec("redact apply /s");
  auto out = sh.Exec("cat /s");
  ASSERT_NE(out.find("***"), std::string::npos);
}

TEST(Bundle, Build){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  ASSERT_EQ(sh.Exec("bundle build vtest key"), "ok");
  auto b64 = sh.Exec("cat /releases/vtest.b64");
  ASSERT_NE(b64.size(), 0u);
  auto sig = sh.Exec("cat /releases/vtest.sig");
  ASSERT_NE(sig.size(), 0u);
}
