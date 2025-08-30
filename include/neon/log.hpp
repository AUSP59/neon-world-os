// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#pragma once
#include <iostream>
#include <string_view>
#include <cstdlib>

namespace neon {
enum class LogLevel { Error=0, Warn=1, Info=2, Debug=3 };

inline LogLevel ParseLogLevel(const char* env){
  if (!env) return LogLevel::Info;
  std::string_view v(env);
  if (v=="debug") return LogLevel::Debug;
  if (v=="info") return LogLevel::Info;
  if (v=="warn") return LogLevel::Warn;
  if (v=="error") return LogLevel::Error;
  return LogLevel::Info;
}

inline LogLevel& GlobalLevel(){
  static LogLevel lvl = ParseLogLevel(std::getenv("NEON_LOG"));
  return lvl;
}

inline void SetLogLevel(LogLevel lvl){ GlobalLevel() = lvl; }

#define NEON_LOG(lvl, msg) do {   if (static_cast<int>(lvl) <= static_cast<int>(::neon::GlobalLevel())) {     std::cerr << "[NEON] " << msg << std::endl;   } } while(0)

} // namespace neon
