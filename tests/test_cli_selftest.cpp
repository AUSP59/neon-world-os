// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <array>
TEST(CLI, SelfTest){
#if defined(_WIN32)
  FILE* pipe = _popen("./build/release/bin/neon --self-test", "r");
#else
  FILE* pipe = popen("./build/release/bin/neon --self-test", "r");
#endif
  ASSERT_NE(pipe, nullptr);
#if defined(_WIN32)
  int rc = _pclose(pipe);
#else
  int rc = pclose(pipe);
#endif
  // Some environments return exit<<8; accept zero as success
  ASSERT_EQ(rc, 0);
}
