#!/usr/bin/env python3
import re, sys, pathlib
root = pathlib.Path(__file__).resolve().parents[2]
cmake = (root/"CMakeLists.txt").read_text(encoding="utf-8", errors="ignore")
m = re.search(r'project\(NEON_WORLD_OS VERSION\s+([0-9]+\.[0-9]+\.[0-9]+)', cmake)
if not m: 
    print("Could not find version in CMakeLists.txt"); sys.exit(1)
cmake_ver = m.group(1)
chg = (root/"CHANGELOG.md").read_text(encoding="utf-8", errors="ignore")
m2 = re.search(r'^##\s+([0-9]+\.[0-9]+\.[0-9]+)\s+-', chg, re.M)
if not m2:
    print("Could not find version in CHANGELOG.md"); sys.exit(1)
chlog_ver = m2.group(1)
if cmake_ver != chlog_ver:
    print(f"Version mismatch: CMakeLists={cmake_ver} vs CHANGELOG={chlog_ver}")
    sys.exit(2)
print("Versions consistent:", cmake_ver)
