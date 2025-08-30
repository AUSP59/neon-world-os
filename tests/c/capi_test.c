\
#include "neon_c.h"
#include <stdio.h>
#include <string.h>
int main(){
  const char* v = neon_c_version();
  if (!v || strlen(v)==0){ fprintf(stderr, "empty version\n"); return 1; }
  printf("NEON C API version: %s\n", v);
  return 0;
}
