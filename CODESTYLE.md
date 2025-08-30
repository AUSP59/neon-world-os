
# C++ Codestyle (short)
- C++20, no exceptions across module boundaries.
- clang-format: see `.clang-format` (LLVM base).
- clang-tidy: enable modernize/cppcoreguidelines/bugprone/performance/readability.
- Public APIs are small, documented, and stable; prefer `string_view` for inputs.
