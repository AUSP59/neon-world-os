
# Packaging Skeletons
## Homebrew (Formula)
```ruby
class NeonWorldOs < Formula
  desc "NEON-WORLD-OS CLI"
  homepage "https://example.invalid"
  url "file://NEON-WORLD-OS_v50.0.0.zip"
  version "50.0.0"
  depends_on "cmake" => :build
  def install
    system "cmake", "-S", ".", "-B", "build", "-DCMAKE_BUILD_TYPE=Release"
    system "cmake", "--build", "build", "-j"
    bin.install "build/release/bin/neon"
  end
end
```

## Debian (debian/control excerpt)
```
Source: neon-world-os
Section: utils
Priority: optional
Build-Depends: debhelper-compat (= 13), cmake, g++
Standards-Version: 4.6.0

Package: neon-world-os
Architecture: any
Depends: ${shlibs:Depends}, ${misc:Depends}
Description: NEON-WORLD-OS CLI
```

## Conan (conanfile.py excerpt)
```python
from conan import ConanFile
class NeonWorldOsConan(ConanFile):
    name = "neon-world-os"
    version = "50.0.0"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"
    exports_sources = "*"
    def build(self):
        self.run("cmake -S . -B build -DCMAKE_BUILD_TYPE=Release")
        self.run("cmake --build build -j")
    def package(self):
        self.copy("neon", dst="bin", src="build/release/bin")
```

> These are starting points for real distribution pipelines.
