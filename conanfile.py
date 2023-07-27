from conan import ConanFile
from conan.tools.cmake import cmake_layout

class CompressorRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def configure(self):
        del self.settings.compiler.libcxx

    def requirements(self):
        self.requires("glfw/3.3.8")
        self.requires("glm/0.9.9.8")
        self.requires("vulkan-headers/1.1.101.0")
        self.requires("tinyobjloader/2.0.0-rc10")
        self.requires("gtest/1.13.0")

    def build_requirements(self):
        self.tool_requires("cmake/3.26.4")

    def layout(self):
        cmake_layout(self, build_folder="conan-build")
