#!/usr/bin/env bash
# SPDX-License-Identifier: MIT
set -euo pipefail
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DBUILD_BENCHMARKS=ON
cmake --build build -j
ctest --test-dir build --output-on-failure
if command -v gcovr >/dev/null; then
  gcovr -r . --html --html-details -o build/coverage.html || true
fi
if command -v doxygen >/dev/null; then
  doxygen docs/api/Doxyfile || true
fi
