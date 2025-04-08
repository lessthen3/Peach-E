from conan import ConanFile
from conan.tools.cmake import cmake_layout
from conan.tools.cmake import CMakeToolchain
from conan.tools.cmake import CMakeDeps

class PeachEBuilder(ConanFile):
    settings = "os", "arch", "compiler", "build_type"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        # just chillin
        self.requires("physfs/3.2.0")

        # geometry
        self.requires("cgal/5.6.1")
        self.requires("clipper2/1.4.0")

        # physics
        self.requires("joltphysics/5.2.0")
        self.requires("box2d/3.0.0")

        # language support
        self.requires("pybind11/2.13.6")
        self.requires("sol2/3.5.0")

        # serialization and compression
        self.requires("zlib/1.3.1")

        # opengl
        self.requires("glm/1.0.1")
        self.requires("glew/2.2.0")

        # 3D asset importing
        self.requires("assimp/5.4.3")

        #encryption/decryption
        self.requires("libsodium/1.0.20")

    def generate(self):
        cmake = CMakeDeps(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def layout(self):
        cmake_layout(self)