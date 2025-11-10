from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, CMakeDeps
import os

class BlinkLite1(ConanFile):
    name = "blinklite1"
    version = "1.0"
    settings = "os", "compiler", "build_type", "arch"
    options = {"emscripten": [True, False]}
    default_options = {"emscripten": False}

    def requirements(self):
        self.requires("sdl/3.2.20", options={"shared": False})
        self.requires("sdl_ttf/3.2.2", options={"shared": False})
        self.requires("sdl_image/3.2.4", options={"shared": False})
        self.requires("glm/1.0.1")
        if self.options.emscripten:
            self.tool_requires("emsdk/3.1.73")
    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        install_dir = os.environ['REPO_DIR']
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        self.run(f"cmake --install . --prefix={install_dir} --config {self.settings.build_type}")