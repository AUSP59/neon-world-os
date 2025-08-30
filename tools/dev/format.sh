#!/usr/bin/env bash
set -euo pipefail
command -v clang-format >/dev/null || (sudo apt-get update && sudo apt-get install -y clang-format)
clang-format -i $(git ls-files '*.[ch]' '*.[ch]pp' '*.[ch]xx')
