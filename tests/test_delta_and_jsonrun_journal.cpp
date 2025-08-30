// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Delta, ExportImport){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo a /x");
  EXPECT_EQ(sh.Exec("snap create A"), "ok");
  sh.Exec("echo b /y");
  EXPECT_EQ(sh.Exec("snap create B"), "ok");
  auto d = sh.Exec("exportfs --delta A B");
  ASSERT_FALSE(d.empty());
  sh.Exec("rm /y");
  EXPECT_EQ(sh.Exec("importdelta " + d), "ok");
  auto st = sh.Exec("stat /y");
  ASSERT_NE(st.find("\"is_dir\":false"), std::string::npos);
}

TEST(Flow, JsonRunAndJournal){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  auto jr = sh.Exec("jsonrun echo ok /a");
  ASSERT_NE(jr.find("\"ok\":"), std::string::npos);
  sh.Exec("journal on journal.log");
  sh.Exec("echo z /zfile");
  sh.Exec("journal off");
  EXPECT_EQ(sh.Exec("journal replay journal.log"), "ok");
}
