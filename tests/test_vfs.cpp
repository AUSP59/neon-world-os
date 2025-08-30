// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/vfs.hpp"

TEST(Vfs, CreateAndList) {
  neon::Vfs vfs;
  EXPECT_TRUE(vfs.Mkdir("/home"));
  EXPECT_TRUE(vfs.Touch("/home/readme.txt"));
  auto ls = vfs.List("/home");
  ASSERT_EQ(ls.size(), 1u);
  EXPECT_EQ(ls[0], "readme.txt");
}

TEST(Vfs, ReadWrite) {
  neon::Vfs vfs;
  EXPECT_TRUE(vfs.Write("/tmp/file.txt", "data"));
  auto d = vfs.Read("/tmp/file.txt");
  ASSERT_TRUE(d.has_value());
  EXPECT_EQ(*d, "data");
}
