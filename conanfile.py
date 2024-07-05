from conan import ConanFile

class PeachEBuilder(ConanFile):
    settings = "os", "arch", "compiler", "build_type"
    generators = "PremakeDeps"

    def requirements(self):
        # just chillin
        self.requires("spdlog/1.14.1")
        self.requires("onetbb/2021.12.0")

        # physics
        self.requires("box2d/2.4.1")
        self.requires("bullet3/3.25")

        # language support
        self.requires("pybind11/2.12.0")
        self.requires("sol2/3.3.1")
        self.requires("angelscript/2.36.1")

        # saving 
        self.requires("nlohmann_json/3.11.3")

        # media controllers
        self.requires("sdl/2.30.4")
        self.requires("bgfx/cci.20230216")
        self.requires("openal-soft/1.23.1")
        self.requires("imgui/cci.20230105+1.89.2.docking")

        # asset importing
        self.requires("stb/cci.20230920")
        self.requires("assimp/5.4.1")
        self.requires("cgltf/1.13")