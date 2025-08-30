// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/vfs.hpp"
TEST(Vfs, Seal){
  neon::Vfs v;
  v.Seal(true);
  EXPECT_FALSE(v.Mkdir("/x"));
  EXPECT_FALSE(v.Write("/x/a","hi"));
  v.Seal(false);
  EXPECT_TRUE(v.Mkdir("/x"));
  EXPECT_TRUE(v.Write("/x/a","hi"));
}
