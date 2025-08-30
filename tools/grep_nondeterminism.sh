#!/usr/bin/env bash
set -euo pipefail
# Fail if __DATE__ or __TIME__ macros appear in sources.
res=$(grep -Rn --include='*.{c,cc,cpp,h,hpp}' -E '__DATE__|__TIME__' . || true)
if [ -n "$res" ]; then
  echo "Found non-deterministic macros:"
  echo "$res"
  exit 1
fi
echo "No nondeterministic macros found."
