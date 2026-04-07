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

///Apple
#include <peach_metal/PeachMetal.hpp>

///SDL
#include <SDL3/SDL_metal.h>

///PeachCore
#include "../../Utils/Logger.h"

extern "C" void PEACH_AssignMetalDeviceToLayer(void* fp_Layer, void* fp_Device);
extern "C" void* PEACH_GetNextDrawable(void* fp_Layer);

namespace PeachCore::Metal{

    struct Renderer
    {
    public:
        struct StatusCode
        {
            static constexpr uint32_t OK                              = 0;
            static constexpr uint32_t FAILED_TO_CREATE_DEVICE        = 1 << 0;
            static constexpr uint32_t FAILED_TO_CREATE_QUEUE         = 1 << 1;
            static constexpr uint32_t FAILED_TO_CREATE_PIPELINE      = 1 << 2;
            static constexpr uint32_t NO_DRAWABLE                    = 1 << 3;
            static constexpr uint32_t FAILED_TO_CREATE_COMMAND_BUFFER = 1 << 4;
        };
    public:
        ~Renderer();

        [[nodiscard]] bool
            Initialize
            (
                SDL_Window* fp_Window,
                shared_ptr<Logger> fp_Logger
            );

        [[nodiscard]] uint32_t BeginFrame();
        [[nodiscard]] uint32_t DrawFrame();
        [[nodiscard]] uint32_t EndFrame();

        void CleanUp();

    private:
        shared_ptr<Logger> rendering_logger = nullptr;

        MTL::Device* pm_Device = nullptr;
        MTL::CommandQueue* pm_CommandQueue  = nullptr;
        MTL::RenderPipelineState* pm_Pipeline = nullptr;

        // per-frame state
        CA::MetalDrawable* pm_CurrentDrawable = nullptr;
        MTL::CommandBuffer* pm_CurrentCommandBuffer = nullptr;
        MTL::RenderCommandEncoder* pm_CurrentEncoder = nullptr;

        // SDL metal view handle -> CAMetalLayer
        SDL_MetalView  pm_MetalView = nullptr;

        // window dims for viewport
        uint32_t pm_WindowWidth  = 0;
        uint32_t pm_WindowHeight = 0;

        bool pm_IsFrameStarted = false;
        
        void* pm_CachedLayer = nullptr;

    private:
        [[nodiscard]] bool CreatePipeline();
    };
}

#endif /*PEACH_RENDERER_METAL*/
