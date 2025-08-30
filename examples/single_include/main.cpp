// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include "neon_world_os.hpp"
int main(){
  neon::Vfs v;
  v.Mkdir("/hi");
  v.Write("/hi/a.txt", "ok");
  return v.Exists("/hi/a.txt") ? 0 : 1;
}
