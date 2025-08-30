
#!/usr/bin/env python3
import os, sys
ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
ok = True
for d in ("include","src","apps","plugins","tests"):
  p = os.path.join(ROOT, d)
  if not os.path.isdir(p): continue
  for r,_,fs in os.walk(p):
    for f in fs:
      if not f.endswith((".h",".hpp",".c",".cc",".cpp",".cxx",".ipp",".inl",".cu",".m",".mm",".py","CMakeLists.txt")): continue
      fp = os.path.join(r,f)
      try:
        with open(fp, "r", encoding="utf-8", errors="ignore") as fh:
          head = fh.read(400)
        if "SPDX-License-Identifier:" not in head:
          print("[SPDX] missing in", os.path.relpath(fp, ROOT))
          ok = False
      except Exception as e:
        print("[SPDX] error", fp, e); ok=False
print("OK" if ok else "FAIL")
sys.exit(0 if ok else 1)
