# NEON-WORLD-OS — Technical Whitepaper (Concise)
**Scope.** Host-embedded OS simulator focused on security, deterministic builds, and top-tier DX.
**Architecture.** Modular C++ core, C ABI shim (`neon_c`), CLI (`neon`), docs & governance.
**Security.** Hardening (RELRO/NOW/FORTIFY/PIE), sanitizers, fuzzing, SBOM (SPDX/CycloneDX), signatures, SLSA, in-toto.
**Reproducibility.** SOURCE_DATE_EPOCH, Nix flake, diffoscope, reprotest, deterministic packaging (CPack).
**Supply Chain.** Scorecard, Dependency Review/OSV, cosign attestations, OCI labels, provenance.
**Governance.** DCO, CODEOWNERS, policies for API stability/backports, SSDF mapping, threat model.
**Operations.** systemd/Kubernetes/seccomp; machine-readable CLI (about/help/trace/doctor/config-validate).
**Portability.** CMake presets/toolchains (x86_64/aarch64/riscv64, Windows/macOS/Linux), devcontainer.
**Roadmap.** Public package repos, real telemetry exporters (OTel/Prom), production benchmarks.
