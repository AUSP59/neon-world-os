
#!/usr/bin/env python3
import os, sys, hashlib, json, time
ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
INCLUDE_DIRS = ["include", "src", "apps", "bindings", "plugins", "LICENSES", "docs"]
def sha256(p):
    h = hashlib.sha256()
    with open(p, "rb") as f:
        for c in iter(lambda: f.read(65536), b""):
            h.update(c)
    return h.hexdigest()
components = []
for d in INCLUDE_DIRS:
    p = os.path.join(ROOT, d)
    if not os.path.isdir(p): continue
    for base, _, files in os.walk(p):
        for fn in files:
            fp = os.path.join(base, fn)
            rel = os.path.relpath(fp, ROOT)
            components.append({
                "type": "file",
                "name": rel.replace('\\','/'),
                "hashes": [{"alg": "SHA-256", "content": sha256(fp)}]
            })
bom = {
    "bomFormat": "CycloneDX",
    "specVersion": "1.5",
    "version": 1,
    "metadata": { "timestamp": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()) },
    "components": components
}
os.makedirs(os.path.join(ROOT, "sbom"), exist_ok=True)
out = os.path.join(ROOT, "sbom", "source.cdx.json")
with open(out, "w") as f: json.dump(bom, f, indent=2)
print(out)
