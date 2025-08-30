#!/usr/bin/env bash
set -euo pipefail
bin="${1:-./build/apps/neon}"
$bin --about-json >/dev/null
$bin --help-json >/dev/null
$bin doctor --json >/dev/null
$bin config --validate --json >/dev/null
$bin --trace-out /tmp/neon-trace.json
test -s /tmp/neon-trace.json || true
echo "E2E OK"
