/*******************************************************************
 *                     Peach Editor v0.0.7
 *             Created by Ranyodh Mandur - 🍑 2024
 *
 *             Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *             https://opensource.org/licenses/MIT
 *
 *      Peach Editor is a free open source editor for Peach-E
********************************************************************/
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
