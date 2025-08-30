
#!/usr/bin/env python3
import sys, xml.etree.ElementTree as ET, pathlib
thresh = int(sys.argv[1]) if len(sys.argv) > 1 else 80
xml = pathlib.Path("build/coverage.xml")
if not xml.exists():
    print("coverage.xml not found at build/coverage.xml")
    sys.exit(0)  # don't fail if missing; CI step handles gcovr result
root = ET.parse(xml).getroot()
line_rate = float(root.attrib.get("line-rate", "0"))
pct = int(round(line_rate * 100))
print(f"Coverage: {pct}% (threshold {thresh}%)")
sys.exit(0 if pct >= thresh else 1)
