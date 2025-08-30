#!/usr/bin/env python3
import os, sys
path, limit = sys.argv[1], int(sys.argv[2])
size = os.path.getsize(path)
print(f"{path} size={size} bytes; limit={limit}")
sys.exit(0 if size <= limit else 1)
