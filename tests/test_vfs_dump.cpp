// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/vfs.hpp"
#include <cstdio>

TEST(VfsDump, Roundtrip) {
  neon::Vfs v;
  ASSERT_TRUE(v.Mkdir("/x"));
  ASSERT_TRUE(v.Write("/x/a.txt", "alpha"));
  ASSERT_TRUE(v.Write("/x/b.txt", "beta"));
  ASSERT_TRUE(v.SaveDump("fsdump.txt"));
  neon::Vfs v2;
  ASSERT_TRUE(v2.LoadDump("fsdump.txt"));
  auto ls = v2.List("/x");
  ASSERT_EQ(ls.size(), 2u);
  auto a = v2.Read("/x/a.txt"); ASSERT_TRUE(a.has_value()); EXPECT_EQ(*a, "alpha");
  std::remove("fsdump.txt");
}
