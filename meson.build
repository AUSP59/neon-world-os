
project('neon-world-os', 'cpp', version: '10.0.0', default_options: ['cpp_std=c++20', 'warning_level=3'])
neon_inc = include_directories('include')
srcs = ['src/memory.cpp','src/scheduler.cpp','src/vfs.cpp','src/shell.cpp','src/util.cpp']
lib = library('neon', srcs, include_directories: neon_inc)
exe = executable('neon', ['apps/neon.cpp'], include_directories: neon_inc, link_with: lib)
test('basic', exe, args: ['-c','help'])
