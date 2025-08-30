#!/usr/bin/env python3
import json, sys, os, re
build_dir = sys.argv[1] if len(sys.argv)>1 else "build"
ccdb = os.path.join(build_dir, "compile_commands.json")
if not os.path.exists(ccdb):
    print("compile_commands.json not found; configure with -DCMAKE_EXPORT_COMPILE_COMMANDS=ON")
    sys.exit(0)
data = json.loads(open(ccdb, "r", encoding="utf-8").read())
oks = 0
for c in data:
    cmd = c.get("command") or " ".join(c.get("arguments", []))
    if any(f in cmd for f in ["-fstack-protector", "-fstack-protector-strong"]) and ("-D_FORTIFY_SOURCE" in cmd or "-U_FORTIFY_SOURCE" not in cmd):
        oks += 1
print(f"{oks} translation units have stack protector and (likely) FORTIFY")
sys.exit(0 if oks>0 else 1)
