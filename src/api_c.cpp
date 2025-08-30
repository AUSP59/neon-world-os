// SPDX-License-Identifier: MIT
#include "neon_c.h"
#include "neon/build_info.hpp"
#include "neon/telemetry.hpp"
#include <string>
#include <cstdlib>
#include <cstring>

extern "C" {

const char* neon_c_version()     { return neon::build::version; }
const char* neon_c_compiler()    { return neon::build::compiler; }
const char* neon_c_build_type()  { return neon::build::type; }
const char* neon_c_std()         { return neon::build::std; }
const char* neon_c_build_date()  { return neon::build::date; }

unsigned long long neon_c_commands_total() {
  return (unsigned long long) neon::Telemetry::instance().commands();
}
unsigned long long neon_c_errors_total() {
  return (unsigned long long) neon::Telemetry::instance().errors();
}

char* neon_c_trace_dump_json() {
  std::string s = neon::Telemetry::instance().dumpTraceJson();
  char* out = (char*) std::malloc(s.size()+1);
  if (!out) return nullptr;
  std::memcpy(out, s.c_str(), s.size()+1);
  return out;
}
void neon_c_free(void* p){ std::free(p); }

} // extern "C"
