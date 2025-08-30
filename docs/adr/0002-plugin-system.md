
# ADR 0002: Runtime Plugin System
Status: Accepted
Context: Extend CLI commands without recompiling core.
Decision: Provide `plugin load <lib>` which loads a shared library exposing `extern "C" void neon_register(Shell&)`.
Consequences: Increases flexibility; add `NEON_ENABLE_PLUGINS` to allow hardened builds without loader.
