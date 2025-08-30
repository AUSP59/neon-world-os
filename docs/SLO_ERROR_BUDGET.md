
# SLOs & Error Budgets
- **Availability**: CLI uptime N/A (local tool). Conformance: `conformance` must return `"ok": true` on release builds.
- **Performance**: p95 latency for core commands (`echo`, `append`, `index`, `search`) under 10ms on reference hardware; enforced via `perf` harness.
- **Security**: `releasegate` must be `"ok": true` on every tagged release.
- **Reliability**: WAL replay produces the last durable state on abnormal termination (validated in smoke tests).
