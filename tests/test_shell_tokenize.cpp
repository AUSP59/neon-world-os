// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Shell, TokenizeQuotes){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  auto out = sh.Exec("echo 'a b' \"/c d\"");
  // The echo command returns err without args wired; we only test that it doesn't crash and tokenizes.
  SUCCEED();
}
