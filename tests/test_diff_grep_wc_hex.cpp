// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Utils, DiffGrepWcHex){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("mkdir /d1"); sh.Exec("mkdir /d2");
  sh.Exec("echo abc /d1/a"); sh.Exec("echo abc /d2/a"); // equal content
  auto js = sh.Exec("diff /d1 /d2");
  ASSERT_NE(js.find("\"content_equal\": true"), std::string::npos);
  sh.Exec("echo x /d2/b");
  auto gp = sh.Exec("grep abc /d1");
  ASSERT_NE(gp.find("/d1/a"), std::string::npos);
  auto wc = sh.Exec("wc /d1/a");
  ASSERT_NE(wc.find("\"bytes\":"), std::string::npos);
  auto hx = sh.Exec("hexdump /d1/a 2");
  ASSERT_EQ(hx.size(), 4u);
}
