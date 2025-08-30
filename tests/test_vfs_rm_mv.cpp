// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/vfs.hpp"

TEST(Vfs, RmMv){
  neon::Vfs v;
  v.Mkdir("/a");
  v.Write("/a/x.txt", "x");
  v.Write("/a/y.txt", "y");
  // non-recursive rm should fail on dir
  EXPECT_FALSE(v.Remove("/a", false));
  // move file and remove dir recursively
  EXPECT_TRUE(v.Move("/a/x.txt", "/x.txt"));
  EXPECT_TRUE(v.Remove("/a", true));
  EXPECT_TRUE(v.Exists("/x.txt"));
  EXPECT_FALSE(v.Exists("/a"));
}
