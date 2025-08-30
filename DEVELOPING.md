# Developing NEON-WORLD-OS
- Build: `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build -j`
- Tests: `ctest --test-dir build --output-on-failure`
- Format: `cmake --build build -t format` or `pre-commit run --all-files`
- Docs: `pip install -r docs/requirements.txt && (cd docs/sphinx && sphinx-build -b html . _build/html)`
