/*******************************************************************
 *                        Peach-E v0.0.1
 *         Created by Ranyodh Singh Mandur - 🍑 2024-2026
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#pragma once

#ifdef PEACH_RENDERER_VULKAN

#include <volk/volk.h>

#include <memory>
#include <stdexcept>

#include <vector>
#include <string>

#include <stdint.h> //always just using the C version of fixed width types owo

namespace PeachCore::Vulkan
{
    using std::string;

    struct DescriptorBindingInfo
    {
        string Name;
        uint32_t Binding;
        uint32_t Set;
        VkDescriptorType Type;
    };

    struct PushConstantInfo
    {
        string Name;
        uint32_t Offset;
        uint32_t Size;
        VkShaderStageFlags StageFlags;
    };

    class PipelineFactory
    {
    public:

        VkGraphicsPipelineCreateInfo
            BakeGraphicsPipelineInfo
            (
                VkDevice device,
                const std::vector<uint32_t>& vertSPV,
                const std::vector<uint32_t>& fragSPV,
                VkRenderPass renderPass,
                VkExtent2D swapchainExtent,
                VkPipelineLayout pipelineLayout
            );
    };
} // namespace PeachEditor

#endif /*PEACH_RENDERER_VULKAN*/
