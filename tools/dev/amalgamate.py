
#!/usr/bin/env python3
import os, re, sys
ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
INCLUDE = os.path.join(ROOT, "include", "neon")
OUTDIR = os.path.join(ROOT, "single_include")
OUT = os.path.join(OUTDIR, "neon_world_os.hpp")

seen = set()
def inline_header(path, out):
    if path in seen: return
    seen.add(path)
    with open(path, "r", encoding="utf-8") as f:
        for line in f:
            m = re.match(r'\s*#include\s+"neon/(.+)"', line)
            if m:
                sub = os.path.join(INCLUDE, m.group(1))
                if os.path.exists(sub):
                    inline_header(sub, out); continue
            out.write(line)

def main():
    os.makedirs(OUTDIR, exist_ok=True)
    headers = [h for h in os.listdir(INCLUDE) if h.endswith(".hpp")]
    with open(OUT, "w", encoding="utf-8") as out:
        out.write("// Amalgamated single-header for NEON-WORLD-OS\n")
        out.write("// SPDX-License-Identifier: MIT\n#pragma once\n")
        # Start from aggregator if exists, else include all
        entry = os.path.join(INCLUDE, "neon.hpp")
        if os.path.exists(entry):
            inline_header(entry, out)
        else:
            for h in sorted(headers):
                inline_header(os.path.join(INCLUDE, h), out)
    print(OUT)

if __name__ == "__main__":
    main()
