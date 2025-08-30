# Threat Model (STRIDE-lite)
- **Spoofing**: signatures on artifacts/images; DCO; Codeowners.
- **Tampering**: SBOMs, cosign attestations, reproducible builds, protected branches (external).
- **Repudiation**: DCO, signed commits/tags (external), CI logs, provenance.
- **Information Disclosure**: secret scanning, minimal logs, REUSE/Licensee checks.
- **Denial of Service**: fuzzing/sanitizers to surface crashes; size and perf guards.
- **Elevation of Privilege**: rootless distroless container, least privilege docs.
Residual risks captured in SECURITY.md.
