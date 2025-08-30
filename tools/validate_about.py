#!/usr/bin/env python3
import json, subprocess, sys
from pathlib import Path
try:
    import jsonschema
except Exception:
    # Attempt local install (CI), fallback to stdlib error otherwise
    import subprocess as sp
    sp.run([sys.executable, "-m", "pip", "install", "--user", "jsonschema"], check=False)
    import jsonschema  # type: ignore

exe = Path(sys.argv[1])
schema_path = Path(sys.argv[2])
out = subprocess.check_output([str(exe), "--about-json"], text=True).strip()
data = json.loads(out)
schema = json.loads(schema_path.read_text(encoding="utf-8"))
jsonschema.validate(data, schema)
print("about-json validated OK")
