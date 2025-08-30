#!/usr/bin/env bash
set -euo pipefail
: "${1:?usage: $0 <artifact-path>}"
artifact="$1"
echo "sha256:"
sha256sum "$artifact" || shasum -a 256 "$artifact"
if command -v cosign >/dev/null 2>&1; then
  echo "cosign verify-blob:"
  cosign verify-blob --signature "$artifact.sig" "$artifact" || echo "Signature missing or invalid"
fi
