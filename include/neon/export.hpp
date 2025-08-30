// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#pragma once
#if defined(_WIN32)
  #if defined(NEON_BUILD_SHARED)
    #define NEON_API __declspec(dllexport)
  #elif defined(NEON_USE_SHARED)
    #define NEON_API __declspec(dllimport)
  #else
    #define NEON_API
  #endif
#else
  #if __GNUC__ >= 4
    #define NEON_API __attribute__((visibility("default")))
  #else
    #define NEON_API
  #endif
#endif
