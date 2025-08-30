// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Shell, ExportImportDryRun){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("mkdir /d"); sh.Exec("echo hi /d/a.txt");
  auto b64 = sh.Exec("exportfs");
  ASSERT_FALSE(b64.empty());
  sh.Exec("rm -r /d");
  EXPECT_EQ(sh.Exec("importfs " + b64), "ok");
  auto st = sh.Exec("stat /d/a.txt");
  ASSERT_NE(st.find("\"is_dir\":false"), std::string::npos);
  sh.Exec("dryrun on");
  auto r = sh.Exec("mkdir /x");  // should not change VFS
  EXPECT_EQ(r, "ok(dryrun)");
  sh.Exec("dryrun off");
}
