#!/usr/bin/env python3
import json, subprocess, sys, tempfile, os
try:
    import jsonschema
except Exception:
    import subprocess as sp
    sp.run([sys.executable, "-m", "pip", "install", "--user", "jsonschema"], check=False)
    import jsonschema  # type: ignore

exe, schema_path = sys.argv[1], sys.argv[2]
p = subprocess.Popen(exe, stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True)
out, _ = p.communicate("trace dump\n", timeout=10)
data = json.loads(out)
schema = json.loads(open(schema_path, "r", encoding="utf-8").read())
jsonschema.validate(data, schema)
print("trace-json validated OK")
