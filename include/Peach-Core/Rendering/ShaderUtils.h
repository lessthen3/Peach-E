#pragma once

#include <volk.h>
#include <Rendering/vk-bootstrap/VkBootstrap.h>

#include "../Managers/LogManager.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace PeachCore {

    struct CompiledShader {
        vector<uint32_t> spirv;
        VkShaderStageFlagBits stage;
    };

    struct GraphicsPipelineCreateInfo
    {
        string VertexShaderPath;
        string FragmentShaderPath;
        VkRenderPass RenderPass;
        VkExtent2D SwapchainExtent;
        VkPipelineLayout PipelineLayout = VK_NULL_HANDLE; // optional override
        VkPrimitiveTopology Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        bool EnableDepthTest = true;
    };

    class ShaderUtils {
    public:
        ShaderUtils() = default;

        // Loads SPIR-V binary from file
        static bool 
            LoadSPIRVFromFile
            (
                const string& path, 
                CompiledShader& outShader, 
                VkShaderStageFlagBits stage
            );

        static VkPipeline
            CreateGraphicsPipeline
            (
                const GraphicsPipelineCreateInfo& createInfo,
                VkDevice device,
                PeachCore::LogManager* logger
            );

        static VkShaderModule
            CreateShaderModule
            (
                VkDevice device,
                const vector<uint32_t>& bytecode
            );

        // Optional: destroy shader module
        void DestroyShaderModule(VkShaderModule module);
    };
}
