// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include "neon/neon_c.h"
#include <stdio.h>
int main(){
  neon_vfs_t* v = neon_vfs_create();
  neon_vfs_mkdir(v, "/demo");
  neon_vfs_write(v, "/demo/hi.txt", "hello");
  char* s = neon_vfs_read(v, "/demo/hi.txt");
  printf("%s\n", s ? s : "(null)");
  neon_free(s);
  neon_vfs_destroy(v);
  return 0;
}
