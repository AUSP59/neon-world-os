
# Data Loss Prevention (DLP) Policy
DLP is disabled by default. When **on**, NEON blocks packaging/export operations if any secret or PII is detected.
- Enable/disable: `dlp on|off|status`
- Test current tree: `dlp test /`
- To tune redaction: use `redact add <regex>` and `anonymize <root>` before exporting.
