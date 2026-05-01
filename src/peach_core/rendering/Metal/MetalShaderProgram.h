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

#ifdef PEACH_RENDERER_METAL

#include <peach_metal/PeachMetal.hpp>
#include <string>

namespace PeachCore::Metal{

    struct GraphicsPipeConfig
    {
        std::string PipelineName;
        MTL::PixelFormat ColorFormat = MTL::PixelFormatBGRA8Unorm;
        // depth, stencil, blend state etc later
    };

    struct ShaderProgram
    {
        GraphicsPipeConfig pm_GraphicsPipe;

        ////////////////////// Shader Functions //////////////////////
        MTL::Function* pm_VertexFunction   = nullptr;
        MTL::Function* pm_FragmentFunction = nullptr;

        ////////////////////// Vertex Layout //////////////////////
        // describes the interleaved vertex buffer layout
        // position, normal, uv, tangent etc
        // metal needs to know the stride and per-attr offsets
        MTL::VertexDescriptor* pm_VertexDescriptor = nullptr;

        ////////////////////// Render Target Format //////////////////////
        MTL::PixelFormat pm_ColorFormat = MTL::PixelFormatBGRA8Unorm;
        MTL::PixelFormat pm_DepthFormat = MTL::PixelFormatDepth32Float;

        ////////////////////// Blending //////////////////////
        bool     pm_BlendEnabled   = false;
        MTL::BlendFactor pm_SrcBlend = MTL::BlendFactorSourceAlpha;
        MTL::BlendFactor pm_DstBlend = MTL::BlendFactorOneMinusSourceAlpha;

        ////////////////////// Argument Table (uniforms/textures) //////////////////////
        // this is where Metal differs from GL
        // no global uniform location queries
        // you define argument buffers or use push-constant-style setVertexBytes
        // for small data like transforms: setVertexBytes(&transform, sizeof, index)
        // for textures: setFragmentTexture(texture, index)
        // for large uniform blocks: MTL::Buffer* bound at a slot
        std::vector<MTL::Buffer*> pm_UniformBuffers; // one per frame in flight typically
    };
}

#endif /*PEACH_RENDERER_METAL*/
