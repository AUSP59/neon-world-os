// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/memory.hpp"
#include <vector>

TEST(Memory, AllocateFree) {
  neon::MemoryManager mm(1024);
  void* p = mm.Allocate(128);
  EXPECT_NE(p, nullptr);
  EXPECT_GT(mm.Used(), 0u);
  mm.Free(p);
  EXPECT_EQ(mm.Used(), 0u);
}
