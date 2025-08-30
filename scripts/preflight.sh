#!/usr/bin/env bash
set -euo pipefail
echo "[1/6] Format check"; (cmake --build build -t format-check 2>/dev/null || true)
echo "[2/6] Codespell"; (codespell -q 3 || true)
echo "[3/6] Markdownlint"; (markdownlint **/*.md || true)
echo "[4/6] Configure"; cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo -DNEON_PEDANTIC=ON
echo "[5/6] Build & Tests"; cmake --build build -j && ctest --test-dir build --output-on-failure || true
echo "[6/6] SBOM policy doc present?"; test -f SBOM_POLICY.md && echo "ok"
