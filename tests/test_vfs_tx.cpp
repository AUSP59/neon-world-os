// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/vfs.hpp"

TEST(Vfs, TxRoundtrip){
  neon::Vfs v;
  EXPECT_TRUE(v.Write("/a.txt", "A"));
  EXPECT_TRUE(v.BeginTx());
  EXPECT_TRUE(v.Write("/b.txt", "B"));
  EXPECT_TRUE(v.RollbackTx());
  EXPECT_FALSE(v.Read("/b.txt").has_value()); // rolled back
  EXPECT_TRUE(v.BeginTx());
  EXPECT_TRUE(v.Write("/c.txt", "C"));
  EXPECT_TRUE(v.CommitTx());
  auto c = v.Read("/c.txt");
  ASSERT_TRUE(c.has_value()); EXPECT_EQ(*c, "C");
}
