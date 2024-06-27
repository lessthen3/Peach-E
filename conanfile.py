from conan import ConanFile

class PeachEBuilder(ConanFile):
    settings = "os", "arch", "compiler", "build_type"
    generators = "PremakeDeps"

    def requirements(self):
        self.requires("spdlog/1.14.1")
        self.requires("box2d/2.4.1")
        self.requires("entt/3.13.0")
        self.requires("imgui/cci.20230105+1.89.2.docking")

        self.requires("pybind11/2.12.0")
        self.requires("sol2/3.3.1")

        self.requires("nlohmann_json/3.11.3")

        self.requires("sdl/2.30.4")
        self.requires("bgfx/cci.20230216")
        self.requires("openal-soft/1.23.1")
        self.requires("stb/cci.20240213")