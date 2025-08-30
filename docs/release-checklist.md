# Release Checklist
- Update VERSION and CHANGELOG.
- Tag `vX.Y.Z` and push.
- Verify CI green (lint, tests, sanitizers, analyzers, coverage gate).
- Check SBOMs and provenance attestations.
- Run `--doctor` on artifacts and verify metadata.
