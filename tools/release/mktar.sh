#!/usr/bin/env bash
set -euo pipefail
: "${1:?usage: $0 <out.tar.gz>}"
out="$1"
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
git ls-files > "$tmp/files"
tar --sort=name --owner=0 --group=0 --numeric-owner     --mtime="@${SOURCE_DATE_EPOCH:-$(date +%s)}"     -czf "$out" -T "$tmp/files"
echo "Wrote $out"
