# Secure C++ Coding Guidelines
- Prefer RAII, `std::unique_ptr`/`std::shared_ptr`; avoid naked `new/delete`.
- Narrowing conversions: explicit; enable UBSan/ASan in CI for undefined behavior detection.
- Validate all untrusted inputs; avoid exceptions across module boundaries unless documented.
- Keep warnings at `-Wall -Wextra -Wpedantic`; fail in CI with `-Werror`.
