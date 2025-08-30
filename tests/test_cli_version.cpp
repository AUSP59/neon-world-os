// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include <array>
#include <cstdio>
#include <string>
TEST(CLI, VersionFlag){
#if defined(_WIN32)
  FILE* pipe = _popen("./build/release/bin/neon --version", "r");
#else
  FILE* pipe = popen("./build/release/bin/neon --version", "r");
#endif
  ASSERT_NE(pipe, nullptr);
  std::array<char,128> buf{}; std::string out;
  while (fgets(buf.data(), buf.size(), pipe)) out += buf.data();
#if defined(_WIN32)
  _pclose(pipe);
#else
  pclose(pipe);
#endif
  ASSERT_FALSE(out.empty());
}
