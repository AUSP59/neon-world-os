#!/usr/bin/env python3
import sys, json
try:
    import yaml
except Exception:
    import subprocess, sys as _s
    subprocess.run([_s.executable, "-m", "pip", "install", "--user", "pyyaml", "jsonschema"], check=False)
    import yaml  # type: ignore
from jsonschema import validate
with open(sys.argv[1], "r", encoding="utf-8") as f:
    doc = yaml.safe_load(f)
schema = json.loads(open(sys.argv[2], "r", encoding="utf-8").read())
validate(instance=doc, schema=schema)
print("k8s manifest validated OK")
