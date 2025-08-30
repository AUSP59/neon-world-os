// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(XAttr, SetGetListMetaHash){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo abc /a");
  EXPECT_EQ(sh.Exec("xattr set /a owner alice"), "ok");
  auto ls = sh.Exec("xattr ls /a");
  ASSERT_NE(ls.find("owner"), std::string::npos);
  auto h1 = sh.Exec("merkleroot /a --meta --xattr");
  EXPECT_EQ(sh.Exec("xattr set /a owner bob"), "ok");
  auto h2 = sh.Exec("merkleroot /a --meta --xattr");
  ASSERT_NE(h1, h2);
}
