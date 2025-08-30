#include "neon/build_info.hpp"
#include <iostream>
int main(){
  std::cout << "NEON version: " << neon::build::version << "\n";
  return 0;
}
