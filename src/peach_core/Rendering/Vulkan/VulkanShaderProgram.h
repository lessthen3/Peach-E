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

///External
#include <volk/volk.h> //THIS NEEDS TO BE FIRST DONT MOVE UWU
#include <vk-bootstrap/VkBootstrap.h>
#include "PipelineFactory.h"

///PeachCore
#include "Utils/Logger.h"
#include "Utils/Serializer.h"

namespace PeachCore::Vulkan { //namespacing this because it doesnt need to be a class, just a file w shader utilization tools

    struct CompiledShader 
    {
        string ShaderName;

        vector<uint32_t> Bytecode;
        VkShaderStageFlagBits Stage;

        //SERIALIZABLE_FIELDS(Bytecode, Stage)
    };

    struct Shaders
    {
        CompiledShader Vertex;
        CompiledShader Fragment;
        CompiledShader Tessalation;
        CompiledShader Compute;
        CompiledShader Geometry;

        //SERIALIZABLE_FIELDS(Vertex, Fragment)
    };

    struct Pipeline
    {
        string PipelineName;

        Shaders ShaderData;

        VkPipelineShaderStageCreateInfo VertexStageInfo = {};
        VkPipelineShaderStageCreateInfo FragStageInfo = {};

        VkPipelineVertexInputStateCreateInfo VertexInputInfo = {};

        VkPipelineInputAssemblyStateCreateInfo InputAssemblyInfo = {};
        VkPipelineRasterizationStateCreateInfo RasterizationInfo = {};

        VkPipelineMultisampleStateCreateInfo MultisampleInfo = {};

        VkPipelineColorBlendAttachmentState ColorBlendAttachment = {};
        VkPipelineColorBlendStateCreateInfo ColorBlendStateInfo = {};

        VkPipelineLayoutCreateInfo PipelineLayoutInfo = {};

        vector<VkDynamicState> DynamicStates;
        VkPipelineDynamicStateCreateInfo DynamicStateInfo = {};

        VkGraphicsPipelineCreateInfo PipelineInfo = {};

        // IMPORTANT: no actual VkShaderModule, VkPipelineLayout, VkPipeline yet

        //SERIALIZABLE_FIELDS(PipelineName, ShaderData)
    };

    struct ShaderProgram
    {
        Pipeline pm_GraphicsPipe;

        bool
            BakePipelineData
            (
                const string& fp_Name,
                Logger* logger
            );

    };
} //namespace PeachCore::Vulkan

#endif /*PEACH_RENDERER_VULKAN*/