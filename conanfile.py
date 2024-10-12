from conan import ConanFile
from conan.tools.cmake import cmake_layout
from conan.tools.cmake import CMakeToolchain
from conan.tools.cmake import CMakeDeps

class PeachEBuilder(ConanFile):
    settings = "os", "arch", "compiler", "build_type"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        # just chillin
        self.requires("onetbb/2021.12.0")
        self.requires("physfs/3.2.0")

        # geometry
        self.requires("cgal/5.6.1")
        self.requires("clipper2/1.3.0")

        # physics
        self.requires("bullet3/3.25")

        # language support
        self.requires("pybind11/2.12.0")
        self.requires("sol2/3.3.1")
        self.requires("angelscript/2.36.1")

        # serialization and compression
        self.requires("nlohmann_json/3.11.3")
        self.requires("cereal/1.3.2")
        self.requires("zlib/1.3.1")

        # opengl
        self.requires("glm/cci.20230113")
        self.requires("glew/2.2.0")
        
        # audio playback
        self.requires("openal-soft/1.22.2")

        # 3D asset importing
        self.requires("assimp/5.4.1")
        self.requires("cgltf/1.13")

    def generate(self):
        cmake = CMakeDeps(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def layout(self):
        cmake_layout(self)