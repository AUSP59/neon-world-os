
#!/usr/bin/env bash
set -euo pipefail
dir="${1:-build}"
find "$dir" -maxdepth 1 -type f \( -name '*.zip' -o -name '*.tar.gz' \) -print0 | while IFS= read -r -d '' f; do
  echo "==> $f"
  sha256sum "$f" || shasum -a 256 "$f"
done
