// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/vfs.hpp"

TEST(Vfs, Quota){
  neon::Vfs v;
  v.SetQuota(5);
  EXPECT_TRUE(v.Write("/a.txt", "12345"));
  EXPECT_FALSE(v.Write("/b.txt", "Z")); // would exceed 5
  v.SetQuota(6);
  EXPECT_TRUE(v.Write("/b.txt", "Z"));
  EXPECT_EQ(v.UsedBytes(), 6u);
}
