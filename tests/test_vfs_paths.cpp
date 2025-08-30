// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/vfs.hpp"

TEST(VfsPaths, Normalize) {
  EXPECT_EQ(neon::Vfs::NormalizePath("/a/./b//c/../d"), "/a/b/d");
  EXPECT_EQ(neon::Vfs::NormalizePath("///"), "/");
}
