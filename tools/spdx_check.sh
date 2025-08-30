#!/usr/bin/env bash
set -euo pipefail
shopt -s globstar nullglob
status=0
for f in **/*.{c,cc,cpp,h,hpp,inl,cmake,py,sh}; do
  grep -E 'SPDX-License-Identifier:' "$f" >/dev/null 2>&1 || { echo "Missing SPDX in $f"; status=1; }
done
exit $status
