#!/usr/bin/env python3
import json, subprocess, sys
try:
    import jsonschema
except Exception:
    import subprocess as sp, sys as _s
    sp.run([_s.executable, "-m", "pip", "install", "--user", "jsonschema"], check=False)
    import jsonschema  # type: ignore

exe, schema = sys.argv[1], sys.argv[2]
out = subprocess.check_output([exe, "doctor", "--json"], text=True).strip()
data = json.loads(out)
schema_data = json.loads(open(schema, "r", encoding="utf-8").read())
jsonschema.validate(data, schema_data)
print("doctor-json validated OK")
