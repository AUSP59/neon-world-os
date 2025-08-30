// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include <gtest/gtest.h>
#include "neon/shell.hpp"
#include "neon/vfs.hpp"
#include "neon/scheduler.hpp"

TEST(Hmac, ExportImportVerify){
  neon::Scheduler s; neon::Vfs v; neon::Shell sh(s,v);
  sh.Exec("echo hi /a");
  auto js = sh.Exec("exportfs --hmac key");
  ASSERT_NE(js.find("\"hmac\""), std::string::npos);
  // naive parse to get fields
  auto bpos = js.find("\"b64\": \""); auto hpos = js.find("\"hmac\": \"");
  ASSERT_NE(bpos, std::string::npos); ASSERT_NE(hpos, std::string::npos);
  auto b64 = js.substr(bpos+8, js.find('"', bpos+8) - (bpos+8) );
  auto mac = js.substr(hpos+9, js.find('"', hpos+9) - (hpos+9) );
  auto r = sh.Exec("importfs --hmac-verify key " + mac + " " + b64);
  ASSERT_EQ(r, "ok");
  auto hv = sh.Exec("hmacverify /a key " + sh.Exec("hmac /a key"));
  ASSERT_EQ(hv, "ok");
}
