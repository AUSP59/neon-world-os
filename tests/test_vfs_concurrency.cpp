// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/vfs.hpp"
#include <thread>
#include <vector>

TEST(Vfs, Concurrency) {
  neon::Vfs v;
  const int N=8, K=100;
  auto worker = [&](int id){
    for (int i=0;i<K;++i){
      std::string p = "/t" + std::to_string(id) + "_" + std::to_string(i);
      v.Touch(p);
      v.Write(p, "x");
      auto r = v.Read(p);
      ASSERT_TRUE(r.has_value());
    }
  };
  std::vector<std::thread> th;
  for (int i=0;i<N;++i) th.emplace_back(worker, i);
  for (auto& t: th) t.join();
  EXPECT_TRUE(v.Exists("/t0_0"));
}
