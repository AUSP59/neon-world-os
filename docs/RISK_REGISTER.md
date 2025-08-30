
# Risk Register (Living Document)
- **Data exfiltration** → Mitigations: DLP gate (`dlp on`), classify/anonymize, release gate
- **Accidental deletion** → Holds/locks/immut/lease, snapshots & WAL replay, FIM baseline
- **Supply chain drift** → Manifest + verify, SBOM generation/diff (existing), packaging skeletons
- **Configuration sprawl** → `harden preset` & `harden show`, policy test runner
