// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/vfs.hpp"

TEST(Vfs, CopyAndStat){
  neon::Vfs v;
  v.Mkdir("/d");
  v.Write("/d/a.txt", "x");
  EXPECT_TRUE(v.Copy("/d/a.txt", "/d/b.txt"));
  bool is_dir=false; std::size_t size=0;
  EXPECT_TRUE(v.Stat("/d/b.txt", is_dir, size));
  EXPECT_FALSE(is_dir);
  EXPECT_EQ(size, 1u);
}
