
# Build & Runtime Hardening
## Build (CMake)
- Enable LTO & RELRO/PIE when available
- Use `-fstack-protector-strong -D_FORTIFY_SOURCE=2 -fno-exceptions -fno-rtti` (where applicable)
- Deterministic builds: honor `SOURCE_DATE_EPOCH`

## Runtime
- Use `harden preset strict` to enable WAL, metrics, watchers, DLP and baseline deny rules.
- Pair with `dlp on`, `oss audit`, `releasegate`, `manifest verify` before tagging a release.
