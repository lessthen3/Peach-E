//#pragma once
//
//#include <vulkan/vulkan.h>
//#include <shaderc/shaderc.hpp>
//#include <string>
//#include <unordered_map>
//#include <vector>
//#include <memory>
//
//namespace PeachCore {
//
//    struct CompiledShader {
//        std::vector<uint32_t> spirv;
//        VkShaderStageFlagBits stage;
//    };
//
//    class ShaderUtils {
//    public:
//        ShaderUtils(VkDevice device, VkDispatchTable dispatch, std::shared_ptr<LogManager> logger)
//            : m_Device(device), m_Dispatch(dispatch), m_Logger(std::move(logger)) {}
//
//        // Compiles GLSL to SPIR-V
//        bool CompileFromGLSL(const std::string& sourceCode, shaderc_shader_kind kind, CompiledShader& outShader, const std::string& fileName);
//
//        // Loads SPIR-V binary from file
//        bool LoadSPIRVFromFile(const std::string& path, CompiledShader& outShader, VkShaderStageFlagBits stage);
//
//        // Creates a Vulkan shader module from SPIR-V
//        VkShaderModule CreateShaderModule(const CompiledShader& compiled);
//
//        // Optional: destroy shader module
//        void DestroyShaderModule(VkShaderModule module);
//
//    private:
//        VkDevice m_Device = VK_NULL_HANDLE;
//        VkDispatchTable m_Dispatch{};
//        std::shared_ptr<LogManager> m_Logger;
//
//        shaderc::Compiler m_Compiler;
//        shaderc::CompileOptions m_Options;
//    };
//}
