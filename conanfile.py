from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout

class NeonWorldOS(ConanFile):
    name = "neon-world-os"
    version = "200.0.0"
    license = "MIT"
    url = "https://example.invalid/neon-world-os"
    description = "Host-embedded OS simulator (C++)"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    exports_sources = "*"
    def layout(self): cmake_layout(self)
    def build(self):
        cm = CMake(self)
        cm.configure()
        cm.build()
    def package(self):
        cm = CMake(self); cm.install()
    def package_info(self):
        self.cpp_info.libs = ["neon"]
