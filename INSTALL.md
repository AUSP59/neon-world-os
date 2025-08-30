# Installation
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
sudo cmake --install build --prefix /usr/local
```
- pkg-config: `neon-world-os-cpp.pc` and `neon-world-os-c.pc`
- CMake package: `find_package(Neon CONFIG REQUIRED)` then `target_link_libraries(... Neon::neon)`
