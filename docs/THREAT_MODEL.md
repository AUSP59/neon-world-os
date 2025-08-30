
# Threat Model
- Assets: in-VFS data, provenance logs, WAL, snapshots, manifests, SBOMs.
- Adversaries: accidental deletion, misconfiguration, insider misuse.
- Controls: immut/lease/holds/locks; safemode & policy gate; WAL & snapshots; SBOM & manifest; quarantine/shred; watchers & audit logs.
- Assumptions: single-node in-process VFS; no network exposure; trust boundary is the operator.
- Residual Risks: operator error bypassing controls; host process compromise; weak host entropy.
- Mitigations: least-privilege operation; backups via `pack`; verify via `manifest` and `verify`.
