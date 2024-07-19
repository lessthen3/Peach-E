from conan import ConanFile

class PeachEBuilder(ConanFile):
    settings = "os", "arch", "compiler", "build_type"
    generators = "PremakeDeps"

    def requirements(self):
        # just chillin
        self.requires("spdlog/1.14.1")
        self.requires("onetbb/2021.12.0")

        # geometry
        self.requires("cgal/5.6.1")
        self.requires("clipper2/1.3.0")

        # physics
        #self.requires("box2d/2.4.1") trying out box2d 3.0 right now
        self.requires("bullet3/3.25")

        # language support
        self.requires("pybind11/2.12.0")
        self.requires("sol2/3.3.1")
        self.requires("angelscript/2.36.1")

        # saving 
        self.requires("nlohmann_json/3.11.3")
        #binary serialization?

        # media controllers
        self.requires("sfml/2.6.1")
        self.requires("imgui/cci.20230105+1.89.2.docking")
        

        # audio decoder and playback
        self.requires("dr_libs/cci.20230529")
        self.requires("openal-soft/1.22.2")

        # asset importing
        self.requires("stb/cci.20230920") #DONT NEED STBI REALLY, I THINK WE CAN REPURPOSE SFML::TEXTURE FOR 3D
        self.requires("assimp/5.4.1")
        self.requires("cgltf/1.13")
        self.requires("nanosvg/cci.20231025")
        
        # # vulkan
        # self.requires("vulkan-memory-allocator/cci.20231120")
        # self.requires("vulkan-headers/1.3.243.0")
        # self.requires("vulkan-loader/1.3.243.0")

        # opengl
        self.requires("glew/2.2.0")
        self.requires("glm/cci.20230113")

