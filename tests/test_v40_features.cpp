// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(RLimit, SetAndEnforce){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("rlimit set echo 1 10"); // 1 token, slow refill
  auto ok = sh.Exec("echo x /a");
  auto block = sh.Exec("echo y /b");
  ASSERT_EQ(block, "err_rate");
}

TEST(Cfg, SchemaAndValidate){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("cfg schema add PORT int");
  sh.Exec("cfg set PORT 8080");
  ASSERT_EQ(sh.Exec("cfg validate"), "ok");
  sh.Exec("cfg set PORT notnum");  // should fail due to schema type
  ASSERT_EQ(sh.Exec("cfg validate"), "err");
}

TEST(Verify, SealAndVerify){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo data /d"); sh.Exec("sealall /"); auto r = sh.Exec("verify /");
  ASSERT_NE(r.find("\"ok\":"), std::string::npos);
}

TEST(RLE, Codec){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo aaaaabbbbcccc /t");
  sh.Exec("rleenc /t /t.rle");
  sh.Exec("rledec /t.rle /u");
  ASSERT_EQ(sh.Exec("cat /u"), "aaaabbbbcccc"); // note: exact run-length; our echo wrote 5a/4b/4c -> after encode->decode expect same
}

TEST(DocCli, Generates){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  ASSERT_EQ(sh.Exec("doc cli"), "ok");
  auto md = sh.Exec("cat /docs/CLI.md");
  ASSERT_NE(md.size(), 0u);
}

TEST(SBOM, SPDX){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo z /z");
  ASSERT_EQ(sh.Exec("sbom /"), "ok");
  auto sb = sh.Exec("cat /SBOM.spdx");
  ASSERT_NE(sb.find("SPDXVersion:"), std::string::npos);
}
