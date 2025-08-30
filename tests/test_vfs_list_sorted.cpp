// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/vfs.hpp"

TEST(Vfs, ListSorted){
  neon::Vfs v;
  v.Mkdir("/z"); v.Mkdir("/a"); v.Mkdir("/m");
  auto ls = v.List("/");
  ASSERT_EQ(ls.size(), 3u);
  EXPECT_EQ(ls[0], "a");
  EXPECT_EQ(ls[1], "m");
  EXPECT_EQ(ls[2], "z");
}
