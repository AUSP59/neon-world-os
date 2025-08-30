// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/merkle.hpp"
#include "neon/crypto.hpp"
#include "neon/vfs.hpp"

TEST(Integrity, MerkleDeterminism){
  neon::Vfs v; v.Mkdir("/d"); v.Write("/d/a","x"); v.Write("/d/b","y");
  auto r1 = neon::MerkleRoot(v);
  auto r2 = neon::MerkleRoot(v);
  ASSERT_EQ(r1.size(), 64u);
  ASSERT_EQ(r1, r2);
  v.Write("/d/b","z");
  auto r3 = neon::MerkleRoot(v);
  ASSERT_NE(r1, r3);
}

TEST(Crypto, HmacLen){
  std::string h = neon::HmacSha256("key", "abc");
  ASSERT_EQ(h.size(), 64u);
  ASSERT_NE(h, neon::HmacSha256("key", "abcd"));
}
