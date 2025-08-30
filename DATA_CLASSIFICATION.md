
# Data Classification
NEON can tag files based on content scanning:
- `classify <root>` sets `tag:class=secret|pii` for matching files.
- Combine with `retpol apply` to set deletion holds for sensitive data.
