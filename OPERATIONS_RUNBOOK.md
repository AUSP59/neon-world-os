# Operations Runbook
- **Health**: `neon --version` for liveness; `--about-json` for inventory.
- **Trace dump**: `neon --trace-out /var/lib/neon/trace.json` (ensure dir writable).
- **Rotation**: rotate logs/SBOM/provenance artifacts each release.
