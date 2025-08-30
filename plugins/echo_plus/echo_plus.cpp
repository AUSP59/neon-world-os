// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include "neon/shell.hpp"
#include <algorithm>
#include <cctype>

extern "C" int neon_plugin_abi(){ return neon::kPluginAbiVersion; }
extern "C" void neon_register(neon::Shell& sh){
  sh.Register("shout", [](const std::vector<std::string>& a){
    if (a.size()<2) return std::string("err");
    std::string s;
    for (size_t i=1;i<a.size();++i){
      if (i>1) s.push_back(' ');
      s += a[i];
    }
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::toupper(c); });
    return s;
  });
}
