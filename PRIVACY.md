# Privacy
NEON-WORLD-OS does **not** send telemetry by default. Metrics/trace features operate **locally**.
- Runtime opt-out: set `NEON_NO_TELEMETRY=1` to disable metrics/trace at runtime.
- Build-time opt-out: compile with `-DNEON_TELEMETRY_DEFAULT_OFF=ON` (if available in your build overlay).
