# Reproducible Release Playbook
- Set `SOURCE_DATE_EPOCH` from the latest annotated tag timestamp.
- Use `cpack` to produce archives; compare using `diffoscope` (see CI job).
- Prefer pinned container/toolchains (devcontainer, Nix flake).
