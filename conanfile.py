from conan import ConanFile
from conan.tools.cmake import cmake_layout


class VulQIanRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    # Add options for build configuration
    options = {
        "build_tests": [True, False],
        "build_examples": [True, False]
    }

    default_options = {
        "build_tests": True,
        "build_examples": True
    }

    def configure(self):
        # Remove libcxx setting for C libraries
        if self.settings.compiler == "gcc":
            if hasattr(self.settings.compiler, "libcxx"):
                del self.settings.compiler.libcxx

    def requirements(self):
        self.requires("glfw/3.4")
        self.requires("glm/1.0.1")
        self.requires("vulkan-headers/1.4.313.0")
        self.requires("tinyobjloader/2.0.0-rc10")

        if self.options.build_tests:
            self.requires("gtest/1.16.0")

    def build_requirements(self):
        self.tool_requires("cmake/3.26.4")

    def layout(self):
        cmake_layout(self, build_folder="conan-build")
