param([string]$Config="Release")
cmake -S . -B build -DCMAKE_BUILD_TYPE=$Config
cmake --build build --config $Config
ctest --test-dir build --output-on-failure
