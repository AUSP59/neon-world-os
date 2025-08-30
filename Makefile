.PHONY: build test install package clean
build:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j
test:
	ctest --test-dir build --output-on-failure
install:
	sudo cmake --install build --prefix /usr/local
package:
	cpack -B dist
clean:
	rm -rf build dist

help:
	@echo "Targets: build, test, install, package, clean"


dist:
	bash tools/release/mktar.sh dist-src.tar.gz || true
	cpack -B dist || true
