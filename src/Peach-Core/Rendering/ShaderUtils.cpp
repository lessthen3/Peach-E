/*******************************************************************
 *                                             Peach-E v0.0.1
 *                           Created by Ranyodh Mandur - 🍑 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                         Peach-E is an open-source game engine
********************************************************************/
#include "../../../include/Peach-Core/Rendering/ShaderUtils.h"

namespace PeachCore {
    namespace ShaderUtils {

        static VkShaderModule
            CreateShaderModule
            (
                const vkb::DispatchTable& fp_DispatchTable,
                const vector<uint32_t>& fp_SpirvBytecode
            )
        {
            VkShaderModuleCreateInfo f_CreateInfo{};
            f_CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            f_CreateInfo.codeSize = fp_SpirvBytecode.size() * sizeof(uint32_t);
            f_CreateInfo.pCode = fp_SpirvBytecode.data();

            VkShaderModule f_ShaderModule;
            if (fp_DispatchTable.createShaderModule(&f_CreateInfo, nullptr, &f_ShaderModule) != VK_SUCCESS)
            {
                return VK_NULL_HANDLE; // failed to create shader module
            }

            return f_ShaderModule;
        }

        static bool
            CreateGraphicsPipeline
            (
                const GraphicsPipelineCreateInfo& createInfo,
                VkDevice device,
                PeachCore::LogManager* logger
            )
        {
            //NEED TO GET RID OF HARD CODED PATH JUST HERE FOR TESTING
            auto vert_code = readFile("D:/Game Development/Peach-E/shaders/triangle.vert.spv");
            auto frag_code = readFile("D:/Game Development/Peach-E/shaders/triangle.frag.spv");

            VkShaderModule vert_module = CreateShaderModule(vert_code);
            VkShaderModule frag_module = CreateShaderModule(frag_code);

            if (vert_module == VK_NULL_HANDLE or frag_module == VK_NULL_HANDLE)
            {
                rendering_logger->LogAndPrint("failed to create shader module, exiting program execution immediately", "VulkanRenderer", LogManager::LogLevel::Fatal);
                return false; // failed to create shader modules
            }

            VkPipelineShaderStageCreateInfo vert_stage_info = {};
            vert_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vert_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
            vert_stage_info.module = vert_module;
            vert_stage_info.pName = "main";

            VkPipelineShaderStageCreateInfo frag_stage_info = {};
            frag_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            frag_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            frag_stage_info.module = frag_module;
            frag_stage_info.pName = "main";

            VkPipelineShaderStageCreateInfo shader_stages[] = { vert_stage_info, frag_stage_info };

            VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
            vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertex_input_info.vertexBindingDescriptionCount = 0;
            vertex_input_info.vertexAttributeDescriptionCount = 0;

            VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
            input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            input_assembly.primitiveRestartEnable = VK_FALSE;

            VkViewport viewport = {};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>(pm_Init.SwapChain.extent.width);
            viewport.height = static_cast<float>(pm_Init.SwapChain.extent.height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            VkRect2D scissor = {};
            scissor.offset = { 0, 0 };
            scissor.extent = pm_Init.SwapChain.extent;

            VkPipelineViewportStateCreateInfo viewport_state = {};
            viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewport_state.viewportCount = 1;
            viewport_state.pViewports = &viewport;
            viewport_state.scissorCount = 1;
            viewport_state.pScissors = &scissor;

            VkPipelineRasterizationStateCreateInfo rasterizer = {};
            rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.depthClampEnable = VK_FALSE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;
            rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
            rasterizer.lineWidth = 1.0f;
            rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
            rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
            rasterizer.depthBiasEnable = VK_FALSE;

            VkPipelineMultisampleStateCreateInfo multisampling = {};
            multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.sampleShadingEnable = VK_FALSE;
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

            VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
            colorBlendAttachment.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = VK_FALSE;

            VkPipelineColorBlendStateCreateInfo color_blending = {};
            color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            color_blending.logicOpEnable = VK_FALSE;
            color_blending.logicOp = VK_LOGIC_OP_COPY;
            color_blending.attachmentCount = 1;
            color_blending.pAttachments = &colorBlendAttachment;
            color_blending.blendConstants[0] = 0.0f;
            color_blending.blendConstants[1] = 0.0f;
            color_blending.blendConstants[2] = 0.0f;
            color_blending.blendConstants[3] = 0.0f;

            VkPipelineLayoutCreateInfo pipeline_layout_info = {};
            pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipeline_layout_info.setLayoutCount = 0;
            pipeline_layout_info.pushConstantRangeCount = 0;

            VkPipelineLayout f_TempLayout;

            if (pm_Init.Dispatch.createPipelineLayout(&pipeline_layout_info, nullptr, &f_TempLayout) != VK_SUCCESS)
            {
                rendering_logger->LogAndPrint("failed to create pipeline layout, exiting program execution immediately", "VulkanRenderer", LogManager::LogLevel::Fatal);
                return false; // failed to create pipeline layout
            }

            pm_RenderData.PipelineLayouts.emplace("name", f_TempLayout);

            vector<VkDynamicState> dynamic_states = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

            VkPipelineDynamicStateCreateInfo dynamic_info = {};
            dynamic_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamic_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
            dynamic_info.pDynamicStates = dynamic_states.data();

            VkGraphicsPipelineCreateInfo pipeline_info = {};
            pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipeline_info.stageCount = 2;
            pipeline_info.pStages = shader_stages;
            pipeline_info.pVertexInputState = &vertex_input_info;
            pipeline_info.pInputAssemblyState = &input_assembly;
            pipeline_info.pViewportState = &viewport_state;
            pipeline_info.pRasterizationState = &rasterizer;
            pipeline_info.pMultisampleState = &multisampling;
            pipeline_info.pColorBlendState = &color_blending;
            pipeline_info.pDynamicState = &dynamic_info;
            pipeline_info.layout = f_TempLayout;
            pipeline_info.renderPass = pm_RenderData.RenderPass;
            pipeline_info.subpass = 0;
            pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

            VkPipeline f_TempGraphicsPipeline;

            if (pm_Init.Dispatch.createGraphicsPipelines(VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &f_TempGraphicsPipeline) != VK_SUCCESS)
            {
                rendering_logger->LogAndPrint("failed to create pipline, exiting program execution immediately", "VulkanRenderer", LogManager::LogLevel::Fatal);
                return false; // failed to create graphics pipeline
            }
            pm_RenderData.GraphicsPipelines.emplace("name", f_TempGraphicsPipeline);

            pm_Init.Dispatch.destroyShaderModule(frag_module, nullptr);
            pm_Init.Dispatch.destroyShaderModule(vert_module, nullptr);

            return true;
        }
    } //namespace ShaderUtils
} //namespace PeachCore