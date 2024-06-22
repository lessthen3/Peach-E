from conan import ConanFile

class MinGEBuilder(ConanFile):
    settings = "os", "arch", "compiler", "build_type"
    generators = "PremakeDeps"

    def requirements(self):
        self.requires("spdlog/1.14.1")
