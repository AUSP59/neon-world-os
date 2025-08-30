// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include <cstdio>
#include <array>
#include <memory>
#include <string>

static std::string run(const std::string& cmd){
#if defined(_WIN32)
  FILE* pipe = _popen(cmd.c_str(), "r");
#else
  FILE* pipe = popen(cmd.c_str(), "r");
#endif
  if (!pipe) return "";
  std::array<char, 256> buf{};
  std::string out;
  while (fgets(buf.data(), buf.size(), pipe)) out += buf.data();
#if defined(_WIN32)
  _pclose(pipe);
#else
  pclose(pipe);
#endif
  return out;
}

TEST(E2E, HelpAndJson) {
  auto help = run("./build/release/bin/neon -c help");
  EXPECT_NE(help.find("Commands:"), std::string::npos);
  auto js = run("./build/release/bin/neon -c ps --json");
  EXPECT_FALSE(js.empty());
}
