// SPDX-License-Identifier: MIT
#include "neon/telemetry.hpp"
#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif
namespace neon {
// Nothing; header-only-ish with small cpp TU for platform includes.
}
