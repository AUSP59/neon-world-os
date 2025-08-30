#!/usr/bin/env bash
set -euo pipefail
: "${1:?Usage: $0 <neon-binary>}"
help2man -N -n "NEON shell" "$1" -o docs/man/neon.1
