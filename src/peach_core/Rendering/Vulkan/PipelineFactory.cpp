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
#ifdef PEACH_RENDERER_VULKAN

#include "PipelineFactory.h"

using namespace std;

namespace PeachCore::Vulkan {

    static VkPipelineLayout 
        CreatePipelineLayout
        (
            VkDevice device,
            const vector<DescriptorBindingInfo>& descriptorBindings,
            const vector<PushConstantInfo>& pushConstants
        )
    {
        vector<VkDescriptorSetLayoutBinding> layoutBindings;
        for (const auto& binding : descriptorBindings)
        {
            VkDescriptorSetLayoutBinding layoutBinding{};
            layoutBinding.binding = binding.Binding;
            layoutBinding.descriptorType = binding.Type;
            layoutBinding.descriptorCount = 1;
            layoutBinding.stageFlags = VK_SHADER_STAGE_ALL;
            layoutBinding.pImmutableSamplers = nullptr;
            layoutBindings.push_back(layoutBinding);
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
        layoutInfo.pBindings = layoutBindings.data();

        VkDescriptorSetLayout descriptorSetLayout;
        if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
        {
            throw runtime_error("Failed to create descriptor set layout!");
        }

        vector<VkPushConstantRange> ranges;
        for (const auto& pc : pushConstants)
        {
            VkPushConstantRange range{};
            range.offset = pc.Offset;
            range.size = pc.Size;
            range.stageFlags = pc.StageFlags;
            ranges.push_back(range);
        }

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(ranges.size());
        pipelineLayoutInfo.pPushConstantRanges = ranges.data();

        VkPipelineLayout pipelineLayout;
        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw runtime_error("Failed to create pipeline layout!");
        }

        return pipelineLayout;
    }

    VkGraphicsPipelineCreateInfo 
        PipelineFactory::BakeGraphicsPipelineInfo
        (
            VkDevice device,
            const vector<uint32_t>& vertSPV,
            const vector<uint32_t>& fragSPV,
            VkRenderPass renderPass,
            VkExtent2D swapchainExtent,
            VkPipelineLayout pipelineLayout
        )
    {
        //VkShaderModule vertModule = CreateShaderModule(device, vertSPV); XXX: idk ab these uwu
        //VkShaderModule fragModule = CreateShaderModule(device, fragSPV);

        VkPipelineShaderStageCreateInfo vertStage{};
        vertStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
        //vertStage.module = vertModule;
        vertStage.pName = "main";

        VkPipelineShaderStageCreateInfo fragStage{};
        fragStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        //fragStage.module = fragModule;
        fragStage.pName = "main";

        VkPipelineShaderStageCreateInfo stages[] = { vertStage, fragStage };

        // Fill the rest of the VkGraphicsPipelineCreateInfo as needed...
        VkGraphicsPipelineCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        info.stageCount = 2;
        info.pStages = stages;
        info.layout = pipelineLayout;
        info.renderPass = renderPass;
        info.subpass = 0;

        // Placeholder: user of this API must fill vertex input, rasterization, etc.
        return info;
    }
}

#endif /*PEACH_RENDERER_VULKAN*/