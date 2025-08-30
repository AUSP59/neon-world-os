#include "neon/build_info.hpp"
#include <cstdio>
int main(){
  std::printf("NEON version: %s\n", neon::build::version);
  return 0;
}
