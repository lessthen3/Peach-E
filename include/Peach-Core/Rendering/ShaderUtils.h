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
#pragma once

#include <volk.h>
#include <Rendering/vk-bootstrap/VkBootstrap.h>

#include "../Managers/LogManager.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace PeachCore {
    namespace ShaderUtils {

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

        static bool
            LoadSPIRVFromFile
            (
                const string& path,
                CompiledShader& outShader,
                VkShaderStageFlagBits stage
            );

        static bool
            CreateGraphicsPipeline
            (
                const GraphicsPipelineCreateInfo& createInfo,
                VkDevice device,
                PeachCore::LogManager* logger
            );

        static VkShaderModule
            CreateShaderModule
            (
                const vkb::DispatchTable& fp_DispatchTable,
                const vector<uint32_t>& fp_SpirvBytecode
            );

        // Optional: destroy shader module
        static void 
            DestroyShaderModule
            (
                VkShaderModule module
            );

    } //namespace ShaderUtils
} //namespace PeachCore
