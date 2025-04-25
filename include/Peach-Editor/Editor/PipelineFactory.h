#pragma once

#include "ShaderCompilerUtils.h" // PeachEditor shader reflection

#include <vulkan/vulkan.h>
#include <memory>
#include <stdexcept>

namespace PeachEditor
{
    
    class PipelineFactory
    {
    public:

        VkGraphicsPipelineCreateInfo
            BakeGraphicsPipelineInfo
            (
                VkDevice device,
                const vector<uint32_t>& vertSPV,
                const vector<uint32_t>& fragSPV,
                VkRenderPass renderPass,
                VkExtent2D swapchainExtent,
                VkPipelineLayout pipelineLayout
            );

    private:
        static VkPipelineLayout 
            CreatePipelineLayout
            (
                VkDevice device,
                const vector<ShaderCompilerUtils::DescriptorBindingInfo>& bindings,
                const vector<ShaderCompilerUtils::PushConstantInfo>& pushConstants
            );
    };
} // namespace PeachEditor
