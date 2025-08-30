// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#pragma once
#include <string>
#include <functional>
#include <filesystem>
#include <cstdlib>

#if defined(_WIN32)
  #include <windows.h>
#else
  #include <dlfcn.h>
#endif

namespace neon {
constexpr int kPluginAbiVersion = 1;


class Shell; // fwd

using RegisterFn = void(*)(Shell&);


inline std::string LoadPlugin(Shell& sh, const std::string& path){
  namespace fs = std::filesystem;
  fs::path p(path);
  // policy: path must be absolute or under NEON_PLUGIN_DIR
  const char* allowed = std::getenv("NEON_PLUGIN_DIR");
  if (!p.is_absolute()){
    if (!allowed) return std::string("err: plugin path must be absolute or under NEON_PLUGIN_DIR");
    fs::path ap = fs::weakly_canonical(fs::path(allowed) / p);
    if (ap.string().find(fs::weakly_canonical(allowed).string()) != 0) {
      return std::string("err: plugin outside NEON_PLUGIN_DIR");
    }
    p = ap;
  }
  std::string full = p.string();

#if defined(_WIN32)
  HMODULE h = LoadLibraryA(full.c_str());
  if (!h) return "err: LoadLibrary failed";
  auto fn = reinterpret_cast<RegisterFn>(GetProcAddress(h, "neon_register"));
  auto abifn = reinterpret_cast<int(*)()>(GetProcAddress(h, "neon_plugin_abi"));
  if (!abifn || abifn() != kPluginAbiVersion) return std::string("err: plugin ABI mismatch");
  if (!fn) return "err: neon_register not found";
  fn(sh);
  return "ok";
#else
  void* h = dlopen(full.c_str(), RTLD_NOW);
  if (!h) return std::string("err: ") + dlerror();
  dlerror();
  auto fn = reinterpret_cast<RegisterFn>(dlsym(h, "neon_register"));
  auto abifn = reinterpret_cast<int(*)()>(dlsym(h, "neon_plugin_abi"));
  if (!abifn || abifn() != kPluginAbiVersion) return std::string("err: plugin ABI mismatch");
  const char* e = dlerror();
  if (e) return std::string("err: ") + e;
  fn(sh);
  return "ok";
#endif
}

} // namespace neon
