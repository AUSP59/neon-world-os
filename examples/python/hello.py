import ctypes, os
lib = ctypes.CDLL('libneon_c.so' if os.name != 'nt' else 'neon_c.dll')
lib.neon_c_version.restype = ctypes.c_char_p
print(lib.neon_c_version().decode())
