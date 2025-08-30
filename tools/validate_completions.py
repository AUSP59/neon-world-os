#!/usr/bin/env python3
import json, subprocess, sys, pathlib
exe, comp_dir = sys.argv[1], sys.argv[2]
help_json = subprocess.check_output([exe, "--help-json"], text=True)
data = json.loads(help_json)
flags = [opt["flag"] for opt in data.get("options", []) if "flag" in opt]
comp = pathlib.Path(comp_dir)
files = list(comp.rglob("*"))
text = "\n".join(p.read_text(errors="ignore") for p in files if p.is_file())
missing = [f for f in flags if f not in text]
if missing:
    print("Missing flags in completions:", ", ".join(missing))
    sys.exit(0)
print("Completions include all known flags.")
