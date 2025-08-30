# NEON-WORLD-OS Whitepaper
## Overview
An auditable, in-memory virtual OS shell with verifiable storage, snapshots, quotas, ACLs, and forensic tooling.
## Threat Model
Non-persistent, single-process; adversary attempts tampering within session. Mitigations include HMAC chains, seals, CAS, attestations, and quotas.
## Verification
Reproducible commands, `sealall` + `verify`, signed snapshots, SBOM (SPDX).