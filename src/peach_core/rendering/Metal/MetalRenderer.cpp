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
#ifdef PEACH_RENDERER_METAL

#include "MetalRenderer.h"
#include <fmt/format.h>

namespace PeachCore::Metal{

    Renderer::~Renderer()
    {
        CleanUp();
    }

    bool
        Renderer::Initialize
        (
            SDL_Window* fp_Window,
            shared_ptr<Logger> fp_Logger
        )
    {
        rendering_logger = fp_Logger;

        ////////////////////// SDL Metal View //////////////////////

        pm_MetalView = SDL_Metal_CreateView(fp_Window);

        if (not pm_MetalView)
        {
            rendering_logger->Fatal("Failed to create SDL Metal view", "MetalRenderer");
            return false;
        }

        ////////////////////// Metal Device //////////////////////

        pm_Device = MTL::CreateSystemDefaultDevice();

        if (not pm_Device)
        {
            rendering_logger->Fatal("Failed to create Metal system default device", "MetalRenderer");
            return false;
        }

        ////////////////////// Assign Device to CAMetalLayer via ObjC bridge //////////////////////

        // SDL_Metal_GetLayer returns a CAMetalLayer* as void*
        // we need the .mm bridge to assign our device to it
        void* f_Layer = SDL_Metal_GetLayer(pm_MetalView);

        if (not f_Layer)
        {
            rendering_logger->Fatal("Failed to get CAMetalLayer from SDL Metal view", "MetalRenderer");
            return false;
        }

        PEACH_AssignMetalDeviceToLayer(f_Layer, pm_Device);

        ////////////////////// Command Queue //////////////////////

        pm_CommandQueue = pm_Device->newCommandQueue();

        if (not pm_CommandQueue)
        {
            rendering_logger->Fatal("Failed to create Metal command queue", "MetalRenderer");
            return false;
        }

        ////////////////////// Window Size //////////////////////

        int f_W = 0;
        int f_H = 0;
        SDL_GetWindowSizeInPixels(fp_Window, &f_W, &f_H);
        pm_WindowWidth  = static_cast<uint32_t>(f_W);
        pm_WindowHeight = static_cast<uint32_t>(f_H);

        ////////////////////// Pipeline //////////////////////

        if (not CreatePipeline())
        {
            return false;
        }

        // cache the layer void* at init time, not per frame
        pm_CachedLayer = SDL_Metal_GetLayer(pm_MetalView);
            
        rendering_logger->Info("Metal renderer initialized successfully UwU", "MetalRenderer");
            
        return true;
    }

    bool
        Renderer::CreatePipeline()
    {
        // compile inline MSL shader source
        // in production you'd load .metallib from your peachbin
        const char* f_ShaderSrc = R"(
            #include <metal_stdlib>
            using namespace metal;

            struct VertexOut
            {
                float4 position [[position]];
                float4 color;
            };

            vertex VertexOut vert_main(uint vertexID [[vertex_id]])
            {
                float2 positions[3] = {
                    float2( 0.0,  0.5),
                    float2(-0.5, -0.5),
                    float2( 0.5, -0.5)
                };
                float4 colors[3] = {
                    float4(1, 0, 0, 1),
                    float4(0, 1, 0, 1),
                    float4(0, 0, 1, 1)
                };
                VertexOut out;
                out.position = float4(positions[vertexID], 0.0, 1.0);
                out.color    = colors[vertexID];
                return out;
            }

            fragment float4 frag_main(VertexOut in [[stage_in]])
            {
                return in.color;
            }
        )";

        NS::Error* f_Error = nullptr;

        NS::String* f_Source = NS::String::string(f_ShaderSrc, NS::StringEncoding::UTF8StringEncoding);

        MTL::CompileOptions* f_Options = MTL::CompileOptions::alloc()->init();

        MTL::Library* f_Library = pm_Device->newLibrary(f_Source, f_Options, &f_Error);

        f_Options->release();

        if (not f_Library)
        {
            rendering_logger->Fatal(
                fmt::format("Failed to compile Metal shaders: {}", f_Error->localizedDescription()->utf8String()),
                "MetalRenderer"
            );
            return false;
        }

        MTL::Function* f_VertFn = f_Library->newFunction(NS::String::string("vert_main", NS::StringEncoding::UTF8StringEncoding));
        MTL::Function* f_FragFn = f_Library->newFunction(NS::String::string("frag_main", NS::StringEncoding::UTF8StringEncoding));

        MTL::RenderPipelineDescriptor* f_PipeDesc = MTL::RenderPipelineDescriptor::alloc()->init();
        f_PipeDesc->setVertexFunction(f_VertFn);
        f_PipeDesc->setFragmentFunction(f_FragFn);
        f_PipeDesc->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm);

        pm_Pipeline = pm_Device->newRenderPipelineState(f_PipeDesc, &f_Error);

        f_VertFn->release();
        f_FragFn->release();
        f_PipeDesc->release();
        f_Library->release();

        if (not pm_Pipeline)
        {
            rendering_logger->Fatal(
                fmt::format("Failed to create Metal pipeline state: {}", f_Error->localizedDescription()->utf8String()),
                "MetalRenderer"
            );
            return false;
        }

        return true;
    }

    uint32_t
        Renderer::BeginFrame()
    {
        if (pm_IsFrameStarted)
        {
            rendering_logger->Warning("BeginFrame called while frame already started", "MetalRenderer");
            return StatusCode::FAILED_TO_CREATE_COMMAND_BUFFER;
        }

        // AutoreleasePool scope per frame — critical for metal-cpp memory management
        NS::AutoreleasePool* f_Pool = NS::AutoreleasePool::alloc()->init();

            // in BeginFrame — no CA::MetalLayer* needed at all
        void* f_RawDrawable = PEACH_GetNextDrawable(pm_CachedLayer);

        if (not f_RawDrawable)
        {
            return StatusCode::NO_DRAWABLE;
        }

        pm_CurrentDrawable = reinterpret_cast<CA::MetalDrawable*>(f_RawDrawable);

        if (not pm_CurrentDrawable)
        {
            f_Pool->release();
            return StatusCode::NO_DRAWABLE;
        }

        pm_CurrentCommandBuffer = pm_CommandQueue->commandBuffer();

        if (not pm_CurrentCommandBuffer)
        {
            f_Pool->release();
            return StatusCode::FAILED_TO_CREATE_COMMAND_BUFFER;
        }

        MTL::RenderPassDescriptor* f_PassDesc = MTL::RenderPassDescriptor::alloc()->init();

        auto* f_Attachment = f_PassDesc->colorAttachments()->object(0);
        f_Attachment->setTexture(pm_CurrentDrawable->texture());
        f_Attachment->setLoadAction(MTL::LoadActionClear);
        f_Attachment->setStoreAction(MTL::StoreActionStore);
        f_Attachment->setClearColor(MTL::ClearColor(0.1, 0.1, 0.1, 1.0)); // dark grey clear

        pm_CurrentEncoder = pm_CurrentCommandBuffer->renderCommandEncoder(f_PassDesc);
        f_PassDesc->release();
        f_Pool->release();

        pm_IsFrameStarted = true;
        return StatusCode::OK;
    }

    uint32_t
        Renderer::DrawFrame()
    {
        if (not pm_IsFrameStarted or not pm_CurrentEncoder)
        {
            return StatusCode::FAILED_TO_CREATE_COMMAND_BUFFER;
        }

        MTL::Viewport f_Viewport =
        {
            0.0, 0.0,
            static_cast<double>(pm_WindowWidth),
            static_cast<double>(pm_WindowHeight),
            0.0, 1.0
        };

        pm_CurrentEncoder->setViewport(f_Viewport);
        pm_CurrentEncoder->setRenderPipelineState(pm_Pipeline);

        // 3 verts, no vertex buffer needed — positions embedded in shader
        pm_CurrentEncoder->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));

        return StatusCode::OK;
    }

    uint32_t
        Renderer::EndFrame()
    {
        if (not pm_IsFrameStarted)
        {
            return StatusCode::FAILED_TO_CREATE_COMMAND_BUFFER;
        }

        pm_CurrentEncoder->endEncoding();
        pm_CurrentCommandBuffer->presentDrawable(pm_CurrentDrawable);
        pm_CurrentCommandBuffer->commit();

        pm_CurrentEncoder       = nullptr;
        pm_CurrentCommandBuffer = nullptr;
        pm_CurrentDrawable      = nullptr;
        pm_IsFrameStarted       = false;

        return StatusCode::OK;
    }

    void
        Renderer::CleanUp()
    {
        if (pm_Pipeline)     { pm_Pipeline->release();     pm_Pipeline     = nullptr; }
        if (pm_CommandQueue) { pm_CommandQueue->release(); pm_CommandQueue = nullptr; }
        if (pm_Device)       { pm_Device->release();       pm_Device       = nullptr; }
        if (pm_MetalView)    { SDL_Metal_DestroyView(pm_MetalView); pm_MetalView = nullptr; }
    }

} // namespace PeachCore::Metal

#endif // PEACH_RENDERER_METAL
