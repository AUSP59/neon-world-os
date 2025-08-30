
# Architecture
- **Core library** (`neon`): VFS, Scheduler, Shell, Utilities.
- **CLI** (`neon` app): REPL and batch command runner using Shell.
- **C API** (`neonc`): C ABI surface on top of C++.
- **Bindings**: optional Python bindings via pybind11.
- **Plugins**: optional runtime commands loaded from shared libs (can be disabled).
- **Security**: policy allowlist, audit log, sealing, sanitizers, static/dynamic analysis, provenance.
- **Packaging**: CPack (ZIP/TGZ/DEB/RPM), pkg-config, Conan/vcpkg templates, Nix/Bazel/Meson.
