// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include "neon/vfs.hpp"
#include <iostream>
int main(){
  neon::Vfs v; v.Mkdir("/demo"); v.Touch("/demo/hi.txt"); v.Write("/demo/hi.txt","hello");
  std::cout << v.Read("/demo/hi.txt").value() << "\n";
  return 0;
}
