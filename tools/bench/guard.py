#!/usr/bin/env python3
import json, sys
# Usage: guard.py <benchmark.json> <thresholds.json>
bench = json.load(open(sys.argv[1]))
thr   = json.load(open(sys.argv[2]))
failed = []
for b in bench.get("benchmarks", []):
    name = b["name"]
    real = b.get("real_time", b.get("cpu_time", 0))
    limit = thr.get(name)
    if limit and real > limit:
        failed.append((name, real, limit))
if failed:
    for (n, r, l) in failed:
        print(f"[REGRESSION] {n}: {r} > {l}")
    sys.exit(1)
print("OK")
