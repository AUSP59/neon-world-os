# CONTRIBUTING to NEON-WORLD-OS

Thank you for improving **NEON-WORLD-OS**! This guide explains how to build, test, and submit high-quality contributions. Please read the **Code of Conduct** first.

---

## 1) Quick Start

```bash
# 1) Fork + clone your fork
git clone https://github.com/<you>/neon-world-os
cd neon-world-os

# 2) Create a topic branch
git checkout -b feat/my-awesome-improvement

# 3) Build (Release) and run tests
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure

# 4) Format + static checks (optional but encouraged)
clang-format -i $(git ls-files '*.[ch]' '*.[ch]pp' '*.cc' 2>/dev/null)
# clang-tidy via compile_commands.json:
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
run-clang-tidy -p build || true

# 5) Commit with DCO sign-off and open a PR
git commit -s -m "feat(cli): add --foo flag for X"
git push origin feat/my-awesome-improvement
2) Project Layout (high-level)
apps/ – CLI entrypoints (e.g., neon.cpp)

include/ – public C/C++ headers (API/ABI surface)

src/ – implementation

lib/ – libraries (e.g., neon_c)

tests/ – unit/integration/e2e tests

schemas/ – JSON Schemas for machine-readable outputs

tools/ – dev/CI utilities (validators, release helpers)

packaging/ – CPack, distro manifests, Dockerfiles

deploy/ – Kubernetes manifests, systemd units

docs/ – Sphinx/Doxygen sources

.github/workflows/ – CI pipelines

3) Build Matrix
Linux (GCC/Clang), macOS (AppleClang), Windows (MSVC).

CMake ≥ 3.20, C++17 (or higher if the CMake preset dictates).

Optional: Nix (flake.nix), devcontainer for reproducible environments.

Useful presets/flags:

bash
Copiar código
# Debug with sanitizers
cmake -S . -B bdebug -DCMAKE_BUILD_TYPE=Debug -DNEON_WERROR=ON
cmake --build bdebug -j

# Link-time optimization & PGO (opt-in)
cmake -S . -B bopt -DCMAKE_BUILD_TYPE=Release -DNEON_ENABLE_LTO=ON -DNEON_ENABLE_PGO=ON

# Static link (use with care; glibc static discouraged)
cmake -S . -B bstatic -DCMAKE_BUILD_TYPE=Release -DNEON_BUILD_STATIC=ON

# Cross-compile examples
cmake -S . -B ba64  -DCMAKE_TOOLCHAIN_FILE=toolchains/aarch64-linux-gnu.cmake
cmake -S . -B brv64 -DCMAKE_TOOLCHAIN_FILE=toolchains/riscv64-linux-gnu.cmake
4) Testing Policy
Unit tests: small, fast, isolated.

Integration tests: exercise modules together.

E2E/contract tests: validate CLI contracts & schemas (e.g., --about-json, --help-json, doctor --json).

Fuzz tests: keep harnesses minimal; avoid flaky assertions.

Determinism: outputs must not depend on wall-clock, locale, or filesystem ordering.

Run select suites:

bash
Copiar código
ctest --test-dir build -R 'help_json_schema|doctor_json_schema|e2e_smoke|install_smoke'
If you add a CLI flag:

Update parsing in apps/neon.cpp.

Update completions in completions/ (bash/zsh/fish/pwsh).

Update or add a JSON Schema if output changes (schemas/).

Add/extend CTest validators in tools/ and wire them in CMakeLists.txt.

5) Code Style & Quality
Formatting: clang-format (LLVM style). Submit formatted code.

Static analysis: clang-tidy is run in CI; fix or justify warnings.

CMake: cmake-format applied in CI.

Headers: add SPDX headers to all source/script files:

cpp
Copiar código
// SPDX-License-Identifier: MIT
Naming & includes:

Header guards or #pragma once.

#include only what you use; prefer forward declarations in headers.

Error handling: never ignore error codes; propagate with context.

Logging/telemetry: respect configuration & privacy (NEON_NO_TELEMETRY, config file).

Performance: prefer move semantics; minimize allocations in hot paths; add micro-benchmarks if relevant.

Threading: avoid data races; document ownership & lifetime.

6) Public API & ABI (C/C++)
The C ABI (neon_c) is the stable surface. Changing it requires:

SONAME/SOVERSION review, symbol map updates, and a CHANGELOG entry.

Tests exercising ABI functions (tests/c_api_smoke.c et al.).

For the C++ API, document stability expectations; avoid exceptions across the C boundary.

Deprecation path:

Mark as deprecated (docs + [[deprecated]] when viable).

Provide alternatives.

Remove in a major release only.

7) Documentation
Update README and docs/ (Sphinx) for user-facing changes.

Keep man page man/neon.1 aligned with new flags.

If you add schemas or config keys, document them in README.md and CONFIGURATION.md.

Keep WHITEPAPER.md and I18N_L10N.md in sync when relevant.

Build docs locally (if Sphinx/Doxygen are installed):

bash
Copiar código
make -C docs/sphinx html || true
doxygen Doxyfile || true
8) Security
Never commit credentials, API keys, or secrets.

For vulnerabilities, follow SECURITY.md (email security@neon.example).

Threat model changes → update the threat model doc and security controls.

Keep SBOMs/provenance pipelines intact; do not remove supply-chain checks.

9) Reproducibility
Honor SOURCE_DATE_EPOCH in builds/archives.

Do not introduce uses of __DATE__/__TIME__ or non-deterministic ordering.

Use provided scripts (tools/release/mktar.sh, determinism guards) as needed.

10) Commit Messages & DCO
We follow Conventional Commits for clarity and automated changelog drafting:

swift
Copiar código
<type>(optional scope): <description>

[optional body]

[optional footer(s)]
Types: feat, fix, docs, test, build, ci, refactor, perf, chore.

DCO sign-off is required:

bash
Copiar código
git commit -s -m "feat(cli): add --foo flag"
This adds:

pgsql
Copiar código
Signed-off-by: Your Name <you@example.com>
11) Pull Requests
Checklist:

 Tests added/updated; ctest passes locally.

 Code formatted; static analysis warnings addressed.

 Docs/manpage/schemas updated when applicable.

 No secrets in diffs; SPDX headers present.

 DCO sign-off (-s) in every commit.

Review expectations:

Two approvals for risky/ABI affecting changes; one approval for small fixes/docs.

Be responsive to feedback; maintainers may push minor fixups (format/docs).

12) Issue Triage (Labels)
type:bug, type:enhancement, type:docs, type:security

area:cli, area:build, area:telemetry, area:packaging, etc.

good first issue, help wanted

priority:low|medium|high, status:needs-info, status:blocked

Please label your issues/PRs to help maintainers triage.

13) Release Hygiene (for maintainers)
Ensure CI is green (non-fatal jobs should still be monitored).

Update CHANGELOG.md, MIGRATION.md if needed.

Bump versions in CMakeLists.txt, packaging manifests where applicable.

Run packaging smoke (cpack_smoke) and install_smoke.

Generate/publish SBOMs & provenance; sign artifacts.

14) Large Changes
For substantial refactors/new components:

Open a design proposal issue describing motivation, alternatives, risks.

Reach rough consensus with maintainers.

Land in small, reviewable PRs.

15) Communication
Use GitHub Issues/PRs/Discussions for technical topics.

Follow the Code of Conduct.

For sensitive topics (conduct/security), use the private emails listed in the respective policies.

16) Contacts
General maintainers: maintainers@neon.example

Security: security@neon.example

Code of Conduct: conduct@neon.example

Thank you for contributing to NEON-WORLD-OS!