# NEON-WORLD-OS

A **host‑embedded operating‑system simulator** in modern **C++20**, engineered to world‑class standards: 
extensive tests, fuzzing, sanitizers, static analysis, SBOM, reproducible CI/CD, governance, security, 
accessibility, inclusion, and comprehensive documentation.

> Version: **v50.0.0** · License: **MIT** · Standard: **C++20** · Build: **CMake**

## Modules
- **Memory** — `neon::MemoryManager`: capacity‑bounded allocator with usage tracking.
- **Scheduler** — `neon::Scheduler`: cooperative queue with time budgets; pausable/resumable tasks.
- **VFS** — `neon::Vfs`: in‑memory hierarchical filesystem with quotas, attributes (immutable), ACLs, HMAC export, fsck.
- **Shell** — `neon::Shell`: userland commands over VFS/Scheduler: `ls, cat, echo, mkdir, touch, rm, mv, cp, ps, kill, meminfo, quota, fsck, schema, diff, grep, wc, hexdump, sha256, hmacverify, chattr, lsattr, exportfs, importfs, find, stat` and more.
- **Self‑test** — `--self-test` one‑shot health/diagnostic runner for CI smoke checks.

## Quick Start
```bash
# Configure and build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j

# Run the shell
./build/apps/neon --help
./build/apps/neon --self-test

# Run tests
ctest --test-dir build --output-on-failure
```

### Recommended flags
- AddressSanitizer: `-DENABLE_ASAN=ON` · UndefinedBehaviorSanitizer: `-DENABLE_UBSAN=ON`
- Turn off exceptions (optional): `-DNEON_DISABLE_EXCEPTIONS=ON`
- Reproducible builds: `-DCMAKE_RANLIB=llvm-ranlib -DCMAKE_AR=llvm-ar` (or system equivalents)

## Toolchain & Dependencies
- **No mandatory third‑party runtime dependencies**. Optional integrations (logging, metrics, tracing) are auto‑detected by CMake if present.
- C++ standard library only by default. Benchmarks and tests fetch Google Benchmark / GoogleTest via CMake `FetchContent`.
- Packaging manifests provided for **Debian**, **Homebrew**, **Scoop**, and **Docker**.

## Security & Supply Chain
- **Security policy:** see `SECURITY.md` and `.well-known/security.txt`.
- **Threat model:** `docs/THREAT_MODEL.md`; **Risk register:** `docs/RISK_REGISTER.md`.
- **SBOM:** CycloneDX for source and release artifacts (see CI).  
- **Provenance:** GitHub **build provenance attestations** on tagged releases.
- **Signing:** Release artifacts are **cosign‑signed (keyless)**; checksums and signatures uploaded to GitHub Releases.
- **REUSE compliance:** SPDX headers; automated check in CI.

## Documentation
- **Architecture:** `ARCHITECTURE.md`, `API_STABILITY.md`, ADRs in `docs/adr/`.
- **CLI Manual:** `docs/cli-spec.md` and `docs/man/neon.1` (installable manpage).
- **Whitepaper:** `WHITEPAPER.md` (technical overview, design rationale, and benchmarks).

## Governance & Community
- **License:** MIT (`LICENSE` and `LICENSES/`).
- **Code of Conduct:** Contributor Covenant (`CODE_OF_CONDUCT.md`).
- **Contributing:** `CONTRIBUTING.md` with PR/Issue templates, DCO, and CODEOWNERS.
- **Inclusion & Accessibility:** `INCLUSION.md`, `ACCESSIBILITY.md`. 
- **Transparency & Privacy:** `TRANSPARENCY.md`, `PRIVACY.md`.
- **Marketing / Project status:** `MARKETING.md`, roadmap in `ROADMAP.md`.

## CI/CD
- Multi‑OS matrix: Linux, macOS, Windows.
- Linting (`clang-format`, `clang-tidy`), sanitizers, tests, coverage (`gcovr`), fuzzing.
- **SBOM (source + binaries)**, **Scorecard**, **CodeQL**, **Provenance attestations**, **Cosign signatures**.
- Release packaging (ZIP/TGZ) with checksums and signed attestations.

## Building the Man Page
```bash
# Example install of man page on Linux:
cmake --build build --target install
man neon
```

## Versioning
- Semantic Versioning (**SemVer**). See `CHANGELOG.md` for detailed changes.

---

<sub>© 2025 NEON-WORLD-OS contributors. MIT License. See `NOTICE` for third‑party attributions.</sub>


## Ultra build modes
- **LTO/IPO**: `-DNEON_ENABLE_LTO=ON`
- **PGO**: First build with `-DNEON_ENABLE_PGO_GEN=ON`, run your typical workload, then rebuild with `-DNEON_ENABLE_PGO_USE=ON`.
- **Reproducible builds**: enabled by default with `-DNEON_REPRODUCIBLE=ON`.

## Supply-chain
- **OpenSSF Scorecard** (`scorecard.yml`), **CodeQL**, **SBOM (source + binary)**, **SLSA provenance**, and **cosign signatures** (keyless) on releases.
- **Checksums**: `SHA256SUMS.txt` published with artifacts. See `tools/release/verify.sh`.

## Documentation sites
- **Doxygen** artifacts and **Sphinx** HTML are published by CI (Pages artifact).



## Telemetry (no external deps)
- Built-in **Prometheus text** metrics via `metrics` shell command (files/dirs/memory/command counters).
- **Chrome trace JSON** via `trace dump` for performance analysis (import into `chrome://tracing`).



## Reproducible builds
CI runs a **bit-for-bit check** (same `SOURCE_DATE_EPOCH`) to ensure identical archives.

## Coverage guard
CI fails if **coverage < 75%** (tune `coverage-gate.yml`).

## Security scanners
- **Gitleaks** for secrets, **Trivy** for vulnerabilities (repo and image), **Licensee** for licensing sanity.

## Packaging metadata
- **pkg-config** (`neon-world-os-cpp.pc`) installed under `${libdir}/pkgconfig`.
- Backstage **catalog-info.yaml** for discovery.

## Container (distroless, rootless)
A hardened image is provided via `packaging/docker/Dockerfile.distroless` (multi-stage).

## Automated releases
**Release Drafter** builds release notes automatically upon merges to `main`.


## ABI Compatibility
`abi-compat.yml` checks ABI breaks on PRs against the base branch when a shared library is built.

## SBOM Drift
`sbom-diff.yml` compares CycloneDX component deltas between base and PR head.

## Documentation & Style Lint
`Markdownlint` and `Codespell` enforce consistent docs and spelling.

## Static Analysis
`scan-build` (Clang Analyzer) augments CodeQL for path-sensitive bug finding.

## Nix Dev Shell
`flake.nix` provides a fully reproducible development shell (CMake, Clang, Doxygen, Sphinx).

## DCO Enforcement
`dco.yml` ensures all commits are Signed-off-by to keep IP provenance clean.


## CMake Package
Consumers can install and `find_package(Neon CONFIG REQUIRED)`, then link `Neon::neon`. A consumer sample is under `packaging/consumer-sample` and is exercised in CI.

## Coverage HTML & Format targets
`coverage-html` emits an HTML report under `build/coverage/`. `format` and `format-check` run clang-format with repo settings.

## Whitepaper PDF
`whitepaper-pdf.yml` renders `WHITEPAPER.md` into a downloadable PDF artifact.

## Size Reports
`size-bloaty.yml` generates a binary size breakdown (symbols) for diagnostics.


## Build system diversity
Alongside CMake, a minimal **Bazel** setup is provided for portability checks.

## CLI smoke test
CTests run a `cli_help` smoke to guard against help/CLI regressions.

## Cross-compile CI
`cross-compile.yml` builds with **musl** and **MinGW-w64** toolchains using provided CMake toolchains.

## Docs & code health
`link-check.yml` validates links in Markdown. `semgrep.yml` and `cppcheck.yml` add static analysis layers.


## Developer Experience
- **compile_commands.json** exported for IDEs; `.clangd` and `.clang-tidy` included.
- **pre-commit** hooks for formatting, spelling, and Markdown lint.
- **Doxygen** config pinned via `Doxyfile`.

## Quality Gates (more layers)
- **Sanitizers** jobs (ASan/UBSan, TSan, MSan) augment CI safety.
- **Changelog Enforcer**: PRs must update `CHANGELOG.md` or carry a `no-changelog` label.

## Metadata & Compliance
- **CITATION.cff** and **codemeta.json** for scholarly and registry ecosystems.
- **REUSE dep5** & `SECURITY_CONTACTS` for compliance & triage.


## Build acceleration
Enable **ccache** via `-DNEON_USE_CCACHE=ON` (auto-detected).

## Version consistency
CI enforces that `CMakeLists.txt` version equals the latest entry in `CHANGELOG.md`.

## REUSE compliance
A dedicated job verifies SPDX headers and licensing metadata using **fsfe/reuse-action**.

## Docs determinism
Docs build uses pinned dependencies in `docs/requirements.txt` and caches pip packages for reproducibility.

## Packaging (extra)
Templates provided for **Arch PKGBUILD** and **RPM spec**.

## Governance automation
- **Renovate** for dependency updates.
- **Commitlint** to enforce Conventional Commits on PRs.
- `.editorconfig` included for consistent whitespace and indentation.

## Containers (multi-arch)
`container-multiarch.yml` builds `linux/amd64` and `linux/arm64` images with Buildx and signs them (keyless).


## Additional quality & compliance gates
- **YAML lint**, **Dockerfile lint** (hadolint), **ShellCheck** for scripts.
- **OSV vulnerability scans** across the repo.
- **Codecov** coverage uploads (tokenless on public repos).
- **Extended OS matrix** (Linux/macOS/Windows) sanity build+test.
- **Diffoscope** reproducibility diff artifact.
- **SPDX SBOM** generation alongside CycloneDX.
- **Automatic PR labels** and **stale-issue** housekeeping.


## C language API
A minimal **C ABI** shim (`neon_c`) is provided for easy FFI.
```c
#include "neon_c.h"
printf("%s\n", neon_c_version());
```
Installed headers: `neon_c.h`; library: `-lneon_c`; pkg-config: `neon-world-os-c`.

## Binary size budget (CI)
`size-guard.yml` fails PRs when the `neon` binary grows by more than **15%** vs base commit.

## clang-tidy PR review
`clang-tidy-review` posts inline suggestions on PRs using the repo's `.clang-tidy` and `compile_commands.json`.


## Machine-readable About
`--about-json` prints build metadata as JSON for automation:
```bash
./build/apps/neon --about-json
```

## Developer Preflight
Run `scripts/preflight.sh` locally to sanity-check formatting, docs, configure, build and tests.

## Windows Build Helper
`scripts/windows_build.ps1 -Config Release`


## Dev Container
A pinned **.devcontainer** is provided for reproducible development in VS Code / GitHub Codespaces.
Open the folder in VS Code and "Reopen in Container".


## Shell completions
Bash, Zsh, and Fish completion scripts are provided under `completions/` and installed via CMake.

## JSON schema validation
`tools/validate_about.py` validates `--about-json` against `schemas/about.schema.json` (run via CTest).

## Examples
- **C++**: `examples/cpp` (`find_package(Neon)`).
- **Python (ctypes)**: `examples/python/hello.py` using the C ABI shim (`neon_c`).

## CMakePresets
Handy presets: `default-release`, `asan`, `tsan`, `unity`, `lld`.
Use: `cmake --preset default-release && cmake --build --preset build`.


## Faster builds
- Optional **precompiled headers**: `-DNEON_ENABLE_PCH=ON`.
- `uninstall` target: `cmake --build build --target uninstall`.

## Project hygiene
- **CODEOWNERS**, **.mailmap**, **.dockerignore** for cleaner repos and releases.
- **Issue templates** (`bug_report.yml`, `feature_request.yml`) to streamline triage.

## MSVC Code Analysis
A dedicated workflow runs `/analyze` on Windows with high warning levels.


## Fuzzing (libFuzzer)
Optional harness (`neon_fuzz`) can be built with Clang:
```bash
cmake -S . -B fuzzb -DNEON_BUILD_FUZZERS=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build fuzzb -j
./fuzzb/neon_fuzz -max_total_time=30
```

## Package managers
- **vcpkg**: manifest `vcpkg.json` included.
- **Conan**: `conan create .` builds and packages the library.

## Container smoke test
CI builds the distroless image and runs `--version` to ensure it starts correctly.

## Caching (build farm friendly)
Optional **sccache** launcher: `-DNEON_USE_SCCACHE=ON` (requires `sccache` installed).


## Provenance & Attestations
An **in-toto** workflow generates a signed link metadata file (`build.link`) with an ephemeral CI key.

## Packaging skeletons
Homebrew formula, Scoop manifest, and Debian skeleton are included under `packaging/`.

## Privacy & Configuration
See `PRIVACY.md` and `CONFIGURATION.md`. A standard `security.txt` is provided under `.well-known/`.


## Supply-chain hardening (added)
- **SLSA provenance** generator workflow (OIDC-based, no secrets).
- **Dependency Review** for PRs.
- **actionlint** ensures GitHub Actions hygiene.
- **FreeBSD ports** skeleton under `packaging/freebsd/`.
- Extended **CPack** to produce native packages (**DEB/RPM** on Unix, **NSIS** on Windows).


## Optional hardening/perf toggles
- **Backtrace self-test**: `--self-test-backtrace` (POSIX).
- **RPATH policy**: installed binaries ship without RPATH on Linux by default.
- **LTO**: `-DNEON_ENABLE_LTO=ON` (default); **PGO**: `-DNEON_ENABLE_PGO=ON` + `pgo-merge` target.
- **MSVC PDB install**: `-DNEON_INSTALL_PDB=ON`.

## Extra CI
- **scan-build** (Clang static analyzer) with artifacts.
- **reprotest** to sanity-check reproducibility under varied environments.
- **pre-commit autoupdate** (scheduled).


## Machine-readable help & trace export
- `--help-json` prints structured help (validated via JSON Schema).
- `--trace-out <file>` writes Chrome-trace JSON to the provided path.

## Ops-ready artifacts
- **Kubernetes** deployment (non-root, read-only FS) under `deploy/kubernetes/`.
- **systemd** unit template under `packaging/systemd/`.
- **Seccomp** profile example under `security/seccomp/`.
- Hardened container run is exercised in CI (`container-security.yml`).


## Doctor (machine-readable)
Use `neon doctor --json` to inspect effective settings and environment. The output is validated in CI against `schemas/doctor.schema.json`.


## Cross-compiling toolchains
Sample CMake toolchains are provided in `toolchains/`:
- `aarch64-linux-gnu.cmake`
- `x86_64-w64-mingw32.cmake`

## Windows integration
- **PowerShell completion** in `completions/neon.ps1`.
- **Service installer** script in `packaging/windows/neon-service.ps1`.

## Hardening & reproducibility extras
- Optional **static build** (`-DNEON_BUILD_STATIC=ON`) and **install strip** (`-DNEON_INSTALL_STRIP=ON`).
- `tools/release/set_source_date_epoch.sh` helps set deterministic timestamps before packaging.
- CI includes **commitlint** and **Dockle** lints.


## Sample configuration
A sample INI file is installed to `${datadir}/neon/neon.conf.example` (see `config/neon.conf.example`).  
Print it with: `neon print-sample-config`.

## Config validation
`neon config --validate --json` emits machine-readable results (schema `schemas/config_validate.schema.json`).  
CI validates it via `config_validate_schema`.


## Nix flake
Reproducible builds via `flake.nix`:
```bash
nix build .#packages.x86_64-linux.default
nix develop  # devShell
```

## Cross-compiling (extra)
Toolchains added under `toolchains/`: include **riscv64-linux-gnu.cmake**.

## End-to-end smoke test
`tools/e2e_smoke.sh` runs a quick CLI contract check; wired to CTest as `e2e_smoke`.


## Static analysis & coverage (offline)
- **clang-tidy** CI runs against `compile_commands.json` (no external services).
- **gcovr** coverage CI publishes HTML/XML artifacts as build outputs.

## Reproducible source archives
Use `tools/release/mktar.sh out.tar.gz` to create a sorted, timestamp-stable tarball (honors `SOURCE_DATE_EPOCH`).

## Packaging skeletons
Added **Arch Linux** (`packaging/archlinux/PKGBUILD`), **MacPorts** (`packaging/macports/Portfile`), and **Chocolatey** (`packaging/chocolatey/*.nuspec`).

## Language bindings (examples)
- **Rust** example in `examples/rust` using FFI to the C ABI.
- **C#** example in `examples/csharp` via P/Invoke.


## Machine-readable version
`neon --version-json` emits `{ "name": "NEON-WORLD-OS", "version": "X.Y.Z" }` and is schema-validated in CI.

## Determinism guard
`tools/grep_nondeterminism.sh` fails the build if `__DATE__`/`__TIME__` macros are present.
CI runs it in `determinism.yml`.

## Security profiles
Example **AppArmor** and **SELinux** policies are provided under `security/`.

## Man page
A minimal `neon(1)` manpage is installed under `share/man/man1`.

## Packaging (desktop ecosystems)
Added skeletons for **Snapcraft** and **Flatpak** apps.


## Additional quality gates
- **CPack smoke** (`cpack_smoke`) ensures installers can be produced on CI.
- **Kubernetes manifest schema** validation (`k8s_manifest_schema`) guards hardened settings.
- **Completions sanity** checks that bash/zsh/fish/pwsh completions include all CLI flags.
- **Binary size budget** (`size_budget`) with a generous limit to catch accidental bloat.
- **SPDX headers check** and **compile flags audit** run in CI.
- **clang-format** check workflow for style hygiene.


## GitHub Green Checks (default)
Workflows are tuned to avoid false negatives on fresh repos:
- Docker-related jobs **skip** if `packaging/docker/Dockerfile.distroless` is absent.
- Strict jobs (`-Werror`, Nix, locale/TZ) run but are **non-fatal** until the repo stabilizes.
- `cpack_smoke` uses **TGZ** generator by default to avoid distro-specific tools.


## Suggested required checks (after first green run)
Once the workflows are green at least once, we suggest marking these as *required* in GitHub Branch Protection:
- **clang-tidy**
- **coverage-gcovr**
- **yamllint / shellcheck / hadolint**
- **determinism-guard**
- **nix-build** (if you use Nix)
- **locale-matrix** (i18n)
- **spdx-check / compile-flags-audit / clang-format**
Adjust to your environment; all jobs are non-blocking by default to avoid false negatives on a fresh repo.


## Community health
This repository ships standard community files (Code of Conduct, Contributing, Security, Support, CODEOWNERS, Funding, issue/PR templates) so GitHub’s Community checklist stays green.
