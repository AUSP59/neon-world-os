#!/usr/bin/env bash
# SPDX-License-Identifier: MIT
set -euo pipefail
if command -v syft >/dev/null; then
  mkdir -p sbom
  syft dir:. -o cyclonedx-json > sbom/neon-world-os-cyclonedx.json
  echo "Generated SBOM into sbom/"
else
  echo "Syft not found; skipping SBOM"
fi
