// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/util.hpp"

TEST(Util, SplitJoin) {
  auto parts = neon::Split("a,b,c", ',');
  ASSERT_EQ(parts.size(), 3u);
  EXPECT_EQ(neon::Join(parts, "-"), "a-b-c");
}
