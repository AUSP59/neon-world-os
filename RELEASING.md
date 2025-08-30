# Releasing
1. Update `CHANGELOG.md` and bump version in `CMakeLists.txt`.
2. Tag `vX.Y.Z` on `main`.
3. CI will build, test, sign artifacts (cosign), generate SBOMs, and draft release notes.
4. Verify signatures with `tools/release/verify.sh`.
Release date target: 2025-08-30
