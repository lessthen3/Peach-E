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

#include "VulkanShaderProgram.h"

namespace PeachCore::Vulkan {

    [[nodiscard]] static VkPipelineLayout 
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

    bool
        ShaderProgram::BakePipelineData
        (
            const string& fp_Name,
            Logger* logger
        )
    {
        if (not logger)
        {

            return false;
        }

        pm_GraphicsPipe.PipelineName = fp_Name;

        pm_GraphicsPipe.VertexStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pm_GraphicsPipe.VertexStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        pm_GraphicsPipe.VertexStageInfo.pName = "main";

        pm_GraphicsPipe.FragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pm_GraphicsPipe.FragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        pm_GraphicsPipe.FragStageInfo.pName = "main";

        pm_GraphicsPipe.VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        pm_GraphicsPipe.VertexInputInfo.vertexBindingDescriptionCount = 0;
        pm_GraphicsPipe.VertexInputInfo.vertexAttributeDescriptionCount = 0;

        pm_GraphicsPipe.InputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        pm_GraphicsPipe.InputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        pm_GraphicsPipe.InputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        pm_GraphicsPipe.RasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        pm_GraphicsPipe.RasterizationInfo.depthClampEnable = VK_FALSE;
        pm_GraphicsPipe.RasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        pm_GraphicsPipe.RasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        pm_GraphicsPipe.RasterizationInfo.lineWidth = 1.0f;
        pm_GraphicsPipe.RasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        pm_GraphicsPipe.RasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        pm_GraphicsPipe.RasterizationInfo.depthBiasEnable = VK_FALSE;

        pm_GraphicsPipe.MultisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        pm_GraphicsPipe.MultisampleInfo.sampleShadingEnable = VK_FALSE;
        pm_GraphicsPipe.MultisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        pm_GraphicsPipe.ColorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        pm_GraphicsPipe.ColorBlendAttachment.blendEnable = VK_FALSE;

        pm_GraphicsPipe.ColorBlendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        pm_GraphicsPipe.ColorBlendStateInfo.logicOpEnable = VK_FALSE;
        pm_GraphicsPipe.ColorBlendStateInfo.logicOp = VK_LOGIC_OP_COPY;
        pm_GraphicsPipe.ColorBlendStateInfo.attachmentCount = 1;
        pm_GraphicsPipe.ColorBlendStateInfo.pAttachments = &pm_GraphicsPipe.ColorBlendAttachment;
        pm_GraphicsPipe.ColorBlendStateInfo.blendConstants[0] = 0.0f;
        pm_GraphicsPipe.ColorBlendStateInfo.blendConstants[1] = 0.0f;
        pm_GraphicsPipe.ColorBlendStateInfo.blendConstants[2] = 0.0f;
        pm_GraphicsPipe.ColorBlendStateInfo.blendConstants[3] = 0.0f;

        pm_GraphicsPipe.PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pm_GraphicsPipe.PipelineLayoutInfo.setLayoutCount = 0;
        pm_GraphicsPipe.PipelineLayoutInfo.pushConstantRangeCount = 0;

        pm_GraphicsPipe.DynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

        pm_GraphicsPipe.DynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        pm_GraphicsPipe.DynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(pm_GraphicsPipe.DynamicStates.size());
        pm_GraphicsPipe.DynamicStateInfo.pDynamicStates = pm_GraphicsPipe.DynamicStates.data();

        pm_GraphicsPipe.PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pm_GraphicsPipe.PipelineInfo.stageCount = 2;
        pm_GraphicsPipe.PipelineInfo.pStages = VK_NULL_HANDLE;
        pm_GraphicsPipe.PipelineInfo.pVertexInputState = &pm_GraphicsPipe.VertexInputInfo;
        pm_GraphicsPipe.PipelineInfo.pInputAssemblyState = &pm_GraphicsPipe.InputAssemblyInfo;
        pm_GraphicsPipe.PipelineInfo.pViewportState = nullptr;
        pm_GraphicsPipe.PipelineInfo.pRasterizationState = &pm_GraphicsPipe.RasterizationInfo;
        pm_GraphicsPipe.PipelineInfo.pMultisampleState = &pm_GraphicsPipe.MultisampleInfo;
        pm_GraphicsPipe.PipelineInfo.pColorBlendState = &pm_GraphicsPipe.ColorBlendStateInfo;
        pm_GraphicsPipe.PipelineInfo.pDynamicState = &pm_GraphicsPipe.DynamicStateInfo;
        pm_GraphicsPipe.PipelineInfo.layout = VK_NULL_HANDLE;
        pm_GraphicsPipe.PipelineInfo.renderPass = VK_NULL_HANDLE;
        pm_GraphicsPipe.PipelineInfo.subpass = 0;
        pm_GraphicsPipe.PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        return true;
    }
} //namespace PeachCore::Vulkan

#endif /*PEACH_RENDERER_VULKAN*/