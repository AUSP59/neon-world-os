// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/vfs.hpp"

TEST(Vfs, FsckJson){
  neon::Vfs v;
  v.Mkdir("/d"); v.Write("/d/a","x");
  auto js = v.FsckJson();
  ASSERT_NE(js.find("\"files\":"), std::string::npos);
  ASSERT_NE(js.find("\"dirs\":"), std::string::npos);
  ASSERT_NE(js.find("\"ok\":"), std::string::npos);
}
