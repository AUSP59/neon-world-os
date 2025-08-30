// SPDX-License-Identifier: MIT
#include "neon/telemetry.hpp"
#include <cstddef>
#include <cstdint>
// Minimal harness: exercises telemetry code paths with random data length to trigger allocations/branches.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  auto& t = neon::Telemetry::instance();
  // simulate N commands/errors bounded by input
  size_t n = size % 1024;
  for (size_t i=0;i<n;i++) { (void)t.commands(); }
  if (size % 2 == 0) { (void)t.errors(); }
  (void)t.dumpTraceJson();
  return 0;
}
