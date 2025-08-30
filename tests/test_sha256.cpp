// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/crypto.hpp"

TEST(Crypto, Sha256Known){
  const char* s = "abc";
  neon::Sha256 h; h.update(s, 3);
  auto hex = neon::Sha256::hex(h.digest());
  EXPECT_EQ(hex, "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}
