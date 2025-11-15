/*******************************************************************
 *                        Peach-E v0.0.1
 *              Created by Ranyodh Mandur - 🍑 2024
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#pragma once

#include <volk.h>
#include <Rendering/vk-bootstrap/VkBootstrap.h>

#include "Logger.h"

#include <unordered_map>
#include <memory>

namespace PeachCore {
namespace ShaderUtils { //namespacing this because it doesnt need to be a class, just a file w shader utilization tools

    struct CompiledShader 
    {
        vector<uint32_t> Bytecode;
        VkShaderStageFlagBits Stage;
    };

    struct BakedPipelineData
    {
        string PipelineName;

        vector<uint32_t> VertexSPV;
        vector<uint32_t> FragmentSPV;

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
    };

    bool
        LoadSPIRVFromFile
        (
            const string& fp_ShaderFilePath,
            vector<uint32_t>& fp_Bytecode,
            Logger* logger
        );

    bool
        BakePipelineData
        (
            const string& fp_Name,
            const string& fp_VertexShaderPath,
            const string& fp_FragShaderPath,
            BakedPipelineData& fp_CreateInfo,
            Logger* logger
        );
} //namespace ShaderUtils
} //namespace PeachCore
