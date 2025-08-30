# ADR-0001: Architecture and Technology Choices

## Status
Accepted

## Context
NEON-WORLD-OS is a host-embedded OS simulator aiming for maximal portability, security, and developer experience.

## Decision
- **Language:** C++20 (upgradable to C++23 when compilers are broadly available).
- **Build:** CMake with presets; multi-OS matrix in CI.
- **Testing:** GoogleTest; Fuzzing with libFuzzer; Benchmarks with Google Benchmark.
- **Packaging:** CPack + distro manifests (Debian, Homebrew, Scoop) and container image.
- **Security & Supply Chain:** SPDX/REUSE, SBOM (CycloneDX), CodeQL, OpenSSF Scorecard, provenance attestations, and cosign signatures.
- **Docs:** Doxygen + Markdown; manpage for CLI.
- **Governance:** Contributor Covenant, DCO, CODEOWNERS.

## Consequences
- Low friction for contributors; reproducible builds across platforms.
- Strong supply-chain signals (SBOM, provenance, signing).
- Clear upgrade path to C++23 without breaking current users.
