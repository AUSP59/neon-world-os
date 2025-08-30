
#include <stdio.h>
#ifdef _WIN32
#define API __declspec(dllimport)
#else
#define API
#endif
extern "C" {
  API const char* neon_c_version();
}
int main(){
  const char* v = neon_c_version ? neon_c_version() : "unknown";
  printf("neon_c_version=%s\n", v ? v : "null");
  return 0;
}
