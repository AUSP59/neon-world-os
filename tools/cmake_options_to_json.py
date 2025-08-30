#!/usr/bin/env python3
import json, os, re, sys, glob
from pathlib import Path
src = Path(sys.argv[1])
schema_path = Path(sys.argv[2])
opts = []
for p in src.rglob("CMakeLists.txt"):
    txt = p.read_text(errors="ignore")
    for m in re.finditer(r'option\(([^)]+)\)', txt):
        line = m.group(1)
        parts = [x.strip() for x in line.split(None, 2)]
        if not parts: continue
        name = parts[0]
        default = None
        desc = ""
        if len(parts) >= 2:
            # Could be "desc" or "ON/OFF"
            if parts[1] in ("ON","OFF"):
                default = parts[1]
            else:
                desc = parts[1]
        if len(parts) == 3:
            if parts[2] in ("ON","OFF"):
                default = parts[2]
            else:
                desc = parts[2]
        opts.append({"name": name, "default": default, "description": desc})
for p in src.rglob("*.cmake"):
    if "build" in str(p): continue
    txt = p.read_text(errors="ignore")
    for m in re.finditer(r'option\(([^)]+)\)', txt):
        line = m.group(1)
        parts = [x.strip() for x in line.split(None, 2)]
        if not parts: continue
        name = parts[0]
        default = None
        desc = ""
        if len(parts) >= 2:
            if parts[1] in ("ON","OFF"):
                default = parts[1]
            else:
                desc = parts[1]
        if len(parts) == 3:
            if parts[2] in ("ON","OFF"):
                default = parts[2]
            else:
                desc = parts[2]
        opts.append({"name": name, "default": default, "description": desc})
data = {"count": len(opts), "options": opts}
print(json.dumps(data))
# optional validate if jsonschema present
try:
    import jsonschema
    jsonschema.validate(data, json.loads(Path(schema_path).read_text()))
except Exception:
    pass
