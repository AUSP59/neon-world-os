// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/neon_c.h"
#include <cstring>

TEST(CAPI, Basic) {
  auto* v = neon_vfs_create();
  ASSERT_NE(v, nullptr);
  EXPECT_TRUE(neon_vfs_mkdir(v, "/demo"));
  EXPECT_TRUE(neon_vfs_write(v, "/demo/hi.txt", "hello"));
  char* s = neon_vfs_read(v, "/demo/hi.txt");
  ASSERT_NE(s, nullptr);
  EXPECT_STREQ(s, "hello");
  neon_free(s);
  neon_vfs_destroy(v);
}
