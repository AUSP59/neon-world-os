// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include <cstdio>
#include <array>
#include <string>

static std::string run(const char* cmd){
#if defined(_WIN32)
  FILE* pipe = _popen(cmd, "r");
#else
  FILE* pipe = popen(cmd, "r");
#endif
  if (!pipe) return {};
  std::array<char,256> b{}; std::string out;
  while (fgets(b.data(), b.size(), pipe)) out += b.data();
#if defined(_WIN32)
  _pclose(pipe);
#else
  pclose(pipe);
#endif
  return out;
}

TEST(CLI, JsonSmoke){
  auto out = run("./build/release/bin/neon -c \"ps\" --json");
  ASSERT_FALSE(out.empty());
  ASSERT_TRUE(out.front()=='{' || out.front()=='[');
}
