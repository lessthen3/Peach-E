/*******************************************************************
 *                                             Peach-E v0.0.1
 *                           Created by Ranyodh Mandur - 🍑 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                     Peach-E is a free open source game engine
********************************************************************/
#include "../../../include/Peach-Core/Utils/ShaderUtils.h"

namespace PeachCore {
namespace ShaderUtils {

    bool
        LoadSPIRVFromFile
        (
            const string& fp_ShaderFilePath,
            vector<uint32_t>& fp_Bytecode,
            LogManager* logger
        )
    {
        if (not logger) //check for nullptr ref passed to ReadBinaryIntoVector
        {
            PrintError("Tried to pass nullptr reference to logger during LoadSPIRVFromFile()");
            return false;
        }

        // Ensure directory exists
        if (not filesystem::exists(fp_ShaderFilePath))
        {
            logger->LogAndPrint("Tried to pass invalid directory to LoadSPIRVFromFile()", "ShaderUtils", LogManager::LogLevel::Error);
            return false;
        }

        if (not fp_Bytecode.empty()) //check if the byte vector is empty before reading data into it OwO
        {
            logger->LogAndPrint(format("Tried passing non-empty byte vector for reading to file name: '{}', nothing was done.", fp_ShaderFilePath), "ShaderUtils", LogManager::LogLevel::Error);
            return false;
        }

        // Extract file extension assuming format "filename.ext"
        size_t lastDotIndex = fp_ShaderFilePath.rfind('.');

        if (lastDotIndex == string::npos)
        {
            logger->LogAndPrint("No file extension found for Peach-E Binary", "ShaderUtils", LogManager::LogLevel::Error);
            return false;
        }

        string f_FileExtension = fp_ShaderFilePath.substr(lastDotIndex);

        if (f_FileExtension != ".spv") //file extension for peach-e binary encoding, get it? it's like a bin of peaches >w<
        {
            logger->LogAndPrint("Attempted to read from a file that isn't a valid SPIRV Binary", "ShaderUtils", LogManager::LogLevel::Error);
            return false;
        }

        ifstream f_ShaderFileHandle(fp_ShaderFilePath, ios::ate | ios::binary);

        if (not f_ShaderFileHandle.is_open())
        {
            logger->LogAndPrint("Failed to open SPIR-V file: " + fp_ShaderFilePath, "ShaderUtils", LogManager::LogLevel::Error);
            return false;
        }

        std::streamsize f_ShaderFileSize = f_ShaderFileHandle.tellg();
        f_ShaderFileHandle.seekg(0, std::ios::beg);

        // Validate size is aligned to 4 bytes
        if (f_ShaderFileSize % sizeof(uint32_t) != 0)
        {
            logger->LogAndPrint("SPIR-V file size is not aligned to 4 bytes", "ShaderUtils", LogManager::LogLevel::Error);
            return false;
        }

        fp_Bytecode.resize(f_ShaderFileSize / sizeof(uint32_t));

        f_ShaderFileHandle.read(reinterpret_cast<char*>(fp_Bytecode.data()), f_ShaderFileSize);
        f_ShaderFileHandle.close();

        return true;
    }

    bool
        BakePipelineData
        (
            const string& fp_Name,
            const string& fp_VertexShaderPath,
            const string& fp_FragShaderPath,
            BakedPipelineData& fp_CreateInfo,
            LogManager* logger
        )
    {
        if (not logger)
        {

            return false;
        }

        fp_CreateInfo.PipelineName = fp_Name;

        if (not LoadSPIRVFromFile(fp_VertexShaderPath, fp_CreateInfo.VertexSPV, logger))
        {

            return false;
        }
        else if (not LoadSPIRVFromFile(fp_FragShaderPath, fp_CreateInfo.FragmentSPV, logger))
        {

            return false;
        }

        fp_CreateInfo.VertexStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fp_CreateInfo.VertexStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        fp_CreateInfo.VertexStageInfo.pName = "main";

        fp_CreateInfo.FragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fp_CreateInfo.FragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fp_CreateInfo.FragStageInfo.pName = "main";

        fp_CreateInfo.VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        fp_CreateInfo.VertexInputInfo.vertexBindingDescriptionCount = 0;
        fp_CreateInfo.VertexInputInfo.vertexAttributeDescriptionCount = 0;

        fp_CreateInfo.InputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        fp_CreateInfo.InputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        fp_CreateInfo.InputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        fp_CreateInfo.RasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        fp_CreateInfo.RasterizationInfo.depthClampEnable = VK_FALSE;
        fp_CreateInfo.RasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        fp_CreateInfo.RasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        fp_CreateInfo.RasterizationInfo.lineWidth = 1.0f;
        fp_CreateInfo.RasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        fp_CreateInfo.RasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        fp_CreateInfo.RasterizationInfo.depthBiasEnable = VK_FALSE;

        fp_CreateInfo.MultisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        fp_CreateInfo.MultisampleInfo.sampleShadingEnable = VK_FALSE;
        fp_CreateInfo.MultisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        fp_CreateInfo.ColorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        fp_CreateInfo.ColorBlendAttachment.blendEnable = VK_FALSE;

        fp_CreateInfo.ColorBlendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        fp_CreateInfo.ColorBlendStateInfo.logicOpEnable = VK_FALSE;
        fp_CreateInfo.ColorBlendStateInfo.logicOp = VK_LOGIC_OP_COPY;
        fp_CreateInfo.ColorBlendStateInfo.attachmentCount = 1;
        fp_CreateInfo.ColorBlendStateInfo.pAttachments = &fp_CreateInfo.ColorBlendAttachment;
        fp_CreateInfo.ColorBlendStateInfo.blendConstants[0] = 0.0f;
        fp_CreateInfo.ColorBlendStateInfo.blendConstants[1] = 0.0f;
        fp_CreateInfo.ColorBlendStateInfo.blendConstants[2] = 0.0f;
        fp_CreateInfo.ColorBlendStateInfo.blendConstants[3] = 0.0f;

        fp_CreateInfo.PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        fp_CreateInfo.PipelineLayoutInfo.setLayoutCount = 0;
        fp_CreateInfo.PipelineLayoutInfo.pushConstantRangeCount = 0;

        fp_CreateInfo.DynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

        fp_CreateInfo.DynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        fp_CreateInfo.DynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(fp_CreateInfo.DynamicStates.size());
        fp_CreateInfo.DynamicStateInfo.pDynamicStates = fp_CreateInfo.DynamicStates.data();

        fp_CreateInfo.PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        fp_CreateInfo.PipelineInfo.stageCount = 2;
        fp_CreateInfo.PipelineInfo.pStages = VK_NULL_HANDLE;
        fp_CreateInfo.PipelineInfo.pVertexInputState = &fp_CreateInfo.VertexInputInfo;
        fp_CreateInfo.PipelineInfo.pInputAssemblyState = &fp_CreateInfo.InputAssemblyInfo;
        fp_CreateInfo.PipelineInfo.pViewportState = nullptr;
        fp_CreateInfo.PipelineInfo.pRasterizationState = &fp_CreateInfo.RasterizationInfo;
        fp_CreateInfo.PipelineInfo.pMultisampleState = &fp_CreateInfo.MultisampleInfo;
        fp_CreateInfo.PipelineInfo.pColorBlendState = &fp_CreateInfo.ColorBlendStateInfo;
        fp_CreateInfo.PipelineInfo.pDynamicState = &fp_CreateInfo.DynamicStateInfo;
        fp_CreateInfo.PipelineInfo.layout = VK_NULL_HANDLE;
        fp_CreateInfo.PipelineInfo.renderPass = VK_NULL_HANDLE;
        fp_CreateInfo.PipelineInfo.subpass = 0;
        fp_CreateInfo.PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        return true;
    }
} //namespace ShaderUtils
} //namespace PeachCore