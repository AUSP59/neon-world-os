#!/usr/bin/env bash
set -euo pipefail
# Use latest annotated tag date or fallback to current date
if git describe --tags --abbrev=0 >/dev/null 2>&1; then
  ts=$(git log -1 --format=%ct "$(git describe --tags --abbrev=0)")
else
  ts=$(date +%s)
fi
export SOURCE_DATE_EPOCH="$ts"
echo "SOURCE_DATE_EPOCH=$SOURCE_DATE_EPOCH"
