# Architecture

- `libneon` (library): memory, scheduler, vfs, shell, util, drivers.
- `neon` (CLI): interactive shell driving the subsystems.
- `tests`: GoogleTest-based unit tests.
- `benchmarks`: Google Benchmark harness.
- `fuzz`: libFuzzer target for `Shell`.
