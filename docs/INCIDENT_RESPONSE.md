
# Incident Response
1. **Detect**: watchers/WAL/metrics anomalies or user report.
2. **Triage**: gather `report`, `metrics dump`, `wal dump`, `snapshot ls`.
3. **Mitigate**: enable `immut`/`lease`/`hold` on affected paths; take `snapshot save`.
4. **Eradicate**: use `quarantine` and `shred` as needed; fix misconfig/policy.
5. **Recover**: `snapshot restore` or `wal replay`; verify with `manifest verify`.
6. **Review**: file a postmortem; update `POLICY.md`/`GOVERNANCE.md`/`SECURITY.md` if needed.
