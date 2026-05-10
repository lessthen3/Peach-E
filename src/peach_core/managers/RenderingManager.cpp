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
#include "RenderingManager.h"
#include "GameManager.h" //this is kosher since it's religated to this TU only owo
#include <memory>

/*
    This class is used to manage the render thread, and queue/unqueue objects safely
    Copyright(c) 2024-present Ranyodh Singh Mandur.

*/

namespace PeachCore {

    //////////////////// Grab and Load Default Texture into Memory UwU ////////////////////

    [[maybe_unused]] [[nodiscard]] static unique_ptr<unsigned char>
        LoadDefaultTexture() 
    {
        [[maybe_unused]] int f_Width = 0, f_Height = 0, f_Channels = 0;

        unique_ptr<unsigned char> f_Pixels = nullptr;

        //(
        //    //stbi_load_from_memory
        //    //(
        //    //    NullResources::PEACH_NULL_TEXTURE,
        //    //    static_cast<int>(NullResources::GetDefaultTextureSize()),
        //    //    &f_Width,
        //    //    &f_Height,
        //    //    &f_Channels,
        //    //    4 // force RGBA
        //    //)
        //);

        if (not f_Pixels)
        {
            //rendering_logger->Error(fmt::format("Failed to load texture default texture! (wtf), reason: {}", stbi_failure_reason()));
            return nullptr;
        }

        return f_Pixels;
    }

}

namespace PeachCore {

    void 
        RenderingManager::Shutdown() //no moar sdl quit only main thread does that owo but it lives the entire runtime and the driver can handle that whatever
    {

    }

    bool 
        RenderingManager::Initialize
        (
            const string& fp_LogOutputDirectory,
            const size_t fp_InitialFrameRate
        )   
    {
        //////////////////// Initialize Logger ////////////////////

        rendering_logger = LogManager::get_single().CreateSharedLogger("RenderingManager", PEACH_LOGGER_DEFAULT_FLAGS, fp_LogOutputDirectory);

        if (not rendering_logger)
        {
            PEACH_PRINT_ERROR("RenderingManager failed to initialize the render_thread logger >w<");
            return false;
        }

        rendering_logger->Debug("RenderingLogger successfully initialized", "RenderingManager");

        //////////////////// Initialize Loading and Command Queues ////////////////////

        if (not InitializeLoadingQueue())
        {
            rendering_logger->Fatal("Initialization failed: RenderingManager was not able to obtain a valid LoadingQueue, exiting execution immediately", "RenderingManager");
            return false;
        }

        InitializeDrawCommandQueue();

        //rendering_logger->Fatal("Invalid rendering backend was selected, RenderingManager was not able to initialize properly", "RenderingManager");

        pm_IsInitialized = true;
        pm_CurrentFrameRateLimit = fp_InitialFrameRate;

        RENDER_FRAME_TIME_STEP = 1.0f / (float)fp_InitialFrameRate;

        return true;
    }
   
    void
        RenderingManager::Stop()
        noexcept
    {
        pm_IsRunning.store(false, std::memory_order_release);
    //         if (this_thread::.joinable()) {
    //     pm_Thread.join(); // <--- This BLOCKS the main thread until the loop finishes
    // }
    }

   bool
        RenderingManager::ProcessCommands()
    {
        bool f_ContainsCommands = false;

        RenderCommand f_Command;
        while (pm_RenderCommandQueue->try_dequeue(f_Command))
        {
            f_ContainsCommands = true;

            switch (f_Command.opcode)
            {
            //case RENDER_CREATE_NODE_OP:   /*CreateNode(cmd.node_id, (ShapeDef*)cmd.operand);*/ break;
            //case RENDER_UPDATE_POSITION_OP: /*UpdatePos(cmd.node_id, UnpackVec2(cmd.operand));*/ break;
            //case RENDER_DONT_DRAW_OP: /*DrawNode(cmd.node_id);*/ break;
            default:
                PEACH_PRINT_ERROR("invalid opcode found for rendering manager! WHAT ARE YE DOIN SON?!?!");
                break;
            }
        }

        return f_ContainsCommands;
    }

    //creates a window and opengl context, enables sfml 2d graphics and such as well, returns the command queue for thread safe control
    bool 
        RenderingManager::InitializeLoadingQueue()
    {
        if (pm_LoadedResourceQueue)
        {
            rendering_logger->Warning("RenderingManager already retrieved the loaded resource queue from ResourceManager >O<", "RenderingManager");
            return false;
        }

        pm_LoadedResourceQueue = ResourceManager::get_single().GetDrawableResourceLoadingQueue(rendering_logger.get());

        if (not pm_LoadedResourceQueue)
        {
            rendering_logger->Error("RenderingManager failed to retrieve LoadingQueue from ResourceManager, nullptr ref was found >O<", "RenderingManager");
            return false;
        }

        rendering_logger->Info("RenderingManager successfully retrieved loaded resource queue from ResourceManager", "RenderingManager");

        return true; //returns one and only one ptr to whoever initializes RenderingManager, this is meant only for the main thread
    }

    bool
        RenderingManager::InitializeDrawCommandQueue()
    {
        if (pm_RenderCommandQueue)
        {
            rendering_logger->Warning("RenderingManager already initialized the draw command queue >O<", "RenderingManager");
            return false;
        }

        pm_RenderCommandQueue = make_shared<RenderCommandPipe>();

        rendering_logger->Info("RenderingManager successfully initialized the draw command queue", "RenderingManager");

        return true; //returns one and only one ptr to whoever initializes RenderingManager, this is meant only for the main thread
    }

    shared_ptr<RenderCommandPipe>
        RenderingManager::GetDrawCommandQueue
        (
            Logger* const logger
        ) //this is supposed to be called from the main thread so cant use the rendering_logger here for thread reasons
    {
        if (not pm_IsInitialized)
        {
            logger->Error("Attempted to get a reference to RenderingManager's DrawCommandQueue before RenderingManager was initialized, please initialize RenderingManager first UwU", "RenderingManager");
            return nullptr;
        }
        else if (pm_RenderCommandQueue.use_count() >= 2)
        {
            logger->Warning("RenderingManager has already issued a reference to the draw command queue, fuck off", "RenderingManager");
            return nullptr;
        }
        
        return pm_RenderCommandQueue;
    }

#ifdef PEACH_RENDERER_OPENGL

    void
        RenderingManager::RenderLoopGL
        (
            const string& fp_LogOutputDirectory,
            latch& fp_InitLatch,
            SDL_Window* fp_MainWindow,
            const uint32_t fp_InitialWindowWidth,
            const uint32_t fp_InitialWindowHeight,
            const size_t fp_InitialFrameRate
        )
    {
        pm_MainWindow = fp_MainWindow;

        if (not Initialize(fp_LogOutputDirectory, fp_InitialFrameRate))
        {

            return;
        }
        if (InitializeOpenGL(fp_InitialWindowWidth, fp_InitialWindowHeight) != PEACH_OK)
        {
            rendering_logger->Fatal("Initialization failed: RenderingManager was not able to create a valid OpenGL context, exiting execution immediately", "RenderingManager");
            GameManager::get_single().ThreadPanicShutdown(PEACH_ERROR_FAILED_TO_INITIALIZE_OPENGL); //not sure if exit should be used here Futur ryan: no it really shouldn't uwu
            pm_IsRunning.store(false, std::memory_order_release);        
        }

        fp_InitLatch.count_down();

        auto f_CurrentTime = chrono::high_resolution_clock::now();
        float f_RenderAccumulator = 0.0f;

        while (pm_IsRunning.load(std::memory_order_acquire))
        {
            auto f_NewTime = chrono::high_resolution_clock::now();
            float f_FrameTime = chrono::duration<float>(f_NewTime - f_CurrentTime).count();

            f_CurrentTime = f_NewTime;

            //////////////////// clamp to one frame render pass when its taking too long owo ////////////////////

            if (f_FrameTime > 0.25)
            {
                f_FrameTime = RENDER_FRAME_TIME_STEP;
            }

            //////////////////// Increment Accumulator ////////////////////

            f_RenderAccumulator += f_FrameTime;

            //////////////////// Physics and fixed interval updates ////////////////////

            if (f_RenderAccumulator >= RENDER_FRAME_TIME_STEP)
            {
                PEACH_TO_DO_UNUSED(PresentFrameGL()); // swap buffers etc.
                f_RenderAccumulator -= RENDER_FRAME_TIME_STEP;
            }

            PEACH_TO_DO_UNUSED(ProcessCommands()); //TODO: write a heuristic to figure out the best way to stream assets since we dont wanna fully drain the pipeline everytime but also tbh doesnt matter that much uwu

            // sleep whatever is left in the budget after all work is done
            float f_Remaining = RENDER_FRAME_TIME_STEP - chrono::duration<float>(chrono::high_resolution_clock::now() - f_CurrentTime).count();

            // sleep the remaining time so we don't burn a core, and be nice to other processes uwu
            if (f_Remaining > 0.001f) // dont bother sleeping < 1ms 
            {
                this_thread::sleep_for(chrono::duration<float>(f_Remaining * 0.9f));  // 90% to avoid oversleeping
            }
        }

        Shutdown();
    }

    bool
        RenderingManager::PresentFrameGL()
    {

        return true;
    }

    PEACH_STATUS_CODE
        RenderingManager::InitializeOpenGL
        (
            const uint32_t fp_InitialWindowWidth,
            const uint32_t fp_InitialWindowHeight
        )
    {
        if (pm_IsOpenGLInitialized)
        {
            rendering_logger->Warning("RenderingManager tried to initialize OpenGL when rendering has already been initialized", "RenderingManager");
            return PEACH_ERROR_FAILED_TO_INITIALIZE_OPENGL;
        }

        rendering_logger->Debug("main SDL window successfully created", "RenderingManager");

        pm_OpenGLRenderer = make_unique<OpenGL::Renderer>(pm_MainWindow, fp_InitialWindowWidth, fp_InitialWindowHeight, rendering_logger, true);

        glewExperimental = GL_TRUE; //????????????????? wtf glew

        if (glewInit() != GLEW_OK)
        {
            rendering_logger->Fatal("Failed to create GLEW context: " + static_cast<string>("OWO"), "RenderingManager");
            //SDL_DestroyWindow(pm_MainWindow);
            return PEACH_ERROR_FAILED_INITIALIZE_GLEW;
        }

        rendering_logger->Debug("GLEW initialized properly", "RenderingManager");

        rendering_logger->Info("Successfully initialized OpenGL!", "RenderingManager");

        pm_IsOpenGLInitialized = true;

        return PEACH_OK;
    }

#endif

#ifdef PEACH_RENDERER_VULKAN

    void
        RenderingManager::RenderLoopVK
        (
            const string& fp_LogOutputDirectory,
            latch& fp_InitLatch,
            SDL_Window* fp_MainWindow,
            const uint32_t fp_InitialWindowWidth,
            const uint32_t fp_InitialWindowHeight,
            const size_t fp_InitialFrameRate
        )
    {
        pm_MainWindow = fp_MainWindow;

        if (not Initialize(fp_LogOutputDirectory, fp_InitialFrameRate))
        {

            return;
        }
        if (not InitializeVulkan(fp_InitialWindowWidth, fp_InitialWindowHeight))
        {
            rendering_logger->Fatal("Initialization failed: RenderingManager was not able to initialize Vulkan, exiting execution immediately", "RenderingManager");
            GameManager::get_single().ThreadPanicShutdown(PEACH_ERROR_FAILED_TO_INITIALIZE_VULKAN);
            pm_IsRunning.store(false, std::memory_order_release);        
        }

        fp_InitLatch.count_down();

        auto f_CurrentTime = chrono::high_resolution_clock::now();
        float f_RenderAccumulator = 0.0f;

        while (pm_IsRunning.load(std::memory_order_acquire))
        {
            auto f_NewTime = chrono::high_resolution_clock::now();
            float f_FrameTime = chrono::duration<float>(f_NewTime - f_CurrentTime).count();

            f_CurrentTime = f_NewTime;

            //////////////////// clamp to one frame render pass when its taking too long owo ////////////////////

            if (f_FrameTime > 0.25)
            {
                f_FrameTime = RENDER_FRAME_TIME_STEP;
            }

            //////////////////// Increment Accumulator ////////////////////

            f_RenderAccumulator += f_FrameTime;

            //////////////////// Physics and fixed interval updates ////////////////////

            if (f_RenderAccumulator >= RENDER_FRAME_TIME_STEP)
            {
                //////////////////// Submit Draw Calls ////////////////////

                uint32_t f_StatusCode = pm_VulkanRenderer->BeginFrame();
                if (f_StatusCode & ~Vulkan::Renderer::StatusCode::OK) //don't even try to draw into cmd buffer or end frame is frame didnt start properly
                {
                    PEACH_PRINT_ERROR_FMT("BeginFrame() failed exit, StatusCode: {}", f_StatusCode);
                }

                f_StatusCode = pm_VulkanRenderer->DrawFrame();
                if (f_StatusCode &~ Vulkan::Renderer::StatusCode::OK)
                {
                    PEACH_PRINT_ERROR_FMT("DrawFrame() failed exit, StatusCode: {}", f_StatusCode);
                }

                f_StatusCode = pm_VulkanRenderer->EndFrame();
                if (f_StatusCode & ~Vulkan::Renderer::StatusCode::OK)
                {
                    PEACH_PRINT_ERROR_FMT("EndFrame() failed exit, StatusCode: {}", f_StatusCode);
                }

                f_RenderAccumulator -= RENDER_FRAME_TIME_STEP;
            }

            PEACH_TO_DO_UNUSED(ProcessCommands()); //TODO: write a heuristic to figure out the best way to stream assets since we dont wanna fully drain the pipeline everytime but also tbh doesnt matter that much uwu

            // sleep whatever is left in the budget after all work is done
            float f_Remaining = RENDER_FRAME_TIME_STEP - chrono::duration<float>(chrono::high_resolution_clock::now() - f_CurrentTime).count();

            // sleep the remaining time so we don't burn a core, and be nice to other processes uwu
            if (f_Remaining > 0.001f) // dont bother sleeping < 1ms 
            {
                this_thread::sleep_for(chrono::duration<float>(f_Remaining * 0.9f));  // 90% to avoid oversleeping
            }
        }

        Shutdown();
    }

    bool
        RenderingManager::InitializeVulkan //yeah ik it copies all the way down the stack frames and is w/e avoids initialization order problems down the line and doesn't really cost anything since this is called once at startup owo
        (
            const uint32_t fp_InitialWindowWidth,
            const uint32_t fp_InitialWindowHeight
        )
    {
        if (volkInitialize() != VK_SUCCESS)
        {
            rendering_logger->Fatal("Volk failed to initialize! ending program execution immediately", "RenderingManager");
            return false;
        }

        pm_VulkanRenderer = make_unique<Vulkan::Renderer>();
        
        if (not pm_VulkanRenderer->Initialize(pm_MainWindow, fp_InitialWindowWidth, fp_InitialWindowHeight, rendering_logger))
        {
            rendering_logger->Fatal("Failed to initialize Vulkan! ending program execution immediately", "RenderingManager");
            return false;
        }

        rendering_logger->Info("Success! VulkanRenderer initialized properly, full rendering capabilities should be ready UwU", "RenderingManager");

        return true; // >w<
    }

#endif

#ifdef PEACH_RENDERER_METAL

    void
        RenderingManager::RenderLoopMetal
        (
            const string& fp_LogOutputDirectory,
            latch& fp_InitLatch,
            SDL_Window* fp_MainWindow,
            const size_t fp_InitialFrameRate
        )
    {
        pm_MainWindow = fp_MainWindow;

        if (not Initialize(fp_LogOutputDirectory, fp_InitialFrameRate))
        {

            return;
        }

        pm_MetalRenderer = make_unique<Metal::Renderer>();

        if (pm_MetalRenderer->Initialize(pm_MainWindow, rendering_logger) != PEACH_OK)
        {
            rendering_logger->Fatal("Initialization failed: RenderingManager was not able to create a valid Metal context, exiting execution immediately", "RenderingManager");
            GameManager::get_single().ThreadPanicShutdown(PEACH_FATAL_ERROR_FAILED_TO_INITIALIZE_METAL); //not sure if exit should be used here
            pm_IsRunning.store(false, std::memory_order_release);        
        }

        fp_InitLatch.count_down();

        auto f_CurrentTime = chrono::high_resolution_clock::now();
        float f_RenderAccumulator = 0.0f;

        while (pm_IsRunning.load(std::memory_order_acquire))
        {
            auto f_NewTime = chrono::high_resolution_clock::now();
            float f_FrameTime = chrono::duration<float>(f_NewTime - f_CurrentTime).count();

            f_CurrentTime = f_NewTime;

            //////////////////// clamp to one frame render pass when its taking too long owo ////////////////////

            if (f_FrameTime > 0.25)
            {
                f_FrameTime = RENDER_FRAME_TIME_STEP;
            }

            //////////////////// Increment Accumulator ////////////////////

            f_RenderAccumulator += f_FrameTime;

            //////////////////// Physics and fixed interval updates ////////////////////

            if (f_RenderAccumulator >= RENDER_FRAME_TIME_STEP)
            {
                //////////////////// Submit Draw Calls ////////////////////

                uint32_t f_StatusCode = pm_MetalRenderer->BeginFrame();
                if (f_StatusCode & ~Metal::Renderer::StatusCode::OK) //don't even try to draw into cmd buffer or end frame is frame didnt start properly
                {
                    PEACH_PRINT_ERROR_FMT("BeginFrame() failed exit, StatusCode: {}", f_StatusCode);
                }

                f_StatusCode = pm_MetalRenderer->DrawFrame();
                if (f_StatusCode &~ Metal::Renderer::StatusCode::OK)
                {
                    PEACH_PRINT_ERROR_FMT("DrawFrame() failed exit, StatusCode: {}", f_StatusCode);
                }

                f_StatusCode = pm_MetalRenderer->EndFrame();
                if (f_StatusCode & ~Metal::Renderer::StatusCode::OK)
                {
                    PEACH_PRINT_ERROR_FMT("EndFrame() failed exit, StatusCode: {}", f_StatusCode);
                }

                f_RenderAccumulator -= RENDER_FRAME_TIME_STEP;
            }

            PEACH_TO_DO_UNUSED(ProcessCommands()); //TODO: write a heuristic to figure out the best way to stream assets since we dont wanna fully drain the pipeline everytime but also tbh doesnt matter that much uwu

            // sleep whatever is left in the budget after all work is done
            float f_Remaining = RENDER_FRAME_TIME_STEP - chrono::duration<float>(chrono::high_resolution_clock::now() - f_CurrentTime).count();

            // sleep the remaining time so we don't burn a core, and be nice to other processes uwu
            if (f_Remaining > 0.001f) // dont bother sleeping < 1ms 
            {
                this_thread::sleep_for(chrono::duration<float>(f_Remaining * 0.9f));  // 90% to avoid oversleeping
            }
        }

        Shutdown();

        #ifdef PEACH_PLATFORM_MACOS
            PEACH_FlushCATransaction(); //commit any pending implicit CA transaction before thread exits, prevents macOS warning on thread deletion
        #endif

        // pm_MetalRenderer->CleanUp();
    }

    PEACH_STATUS_CODE
        RenderingManager::InitializeMetal()
    {

        return PEACH_OK;
    }

    bool
        RenderingManager::PresentFrameMetal()
    {

        return true;
    }

#endif

    void 
        RenderingManager::ResizeWindow()
    {

    }

    void
        RenderingManager::GetCurrentViewPort()
    {
      
    }

    size_t
        RenderingManager::GetCurrentFrameRateLimit() 
        const noexcept
    {
        return pm_CurrentFrameRateLimit;
    }

    void
        RenderingManager::SetNewFrameRateLimit(const size_t fp_NewFrameRateLimit)
        noexcept
    {
        pm_CurrentFrameRateLimit = fp_NewFrameRateLimit;
        RENDER_FRAME_TIME_STEP = 1.0f / (float)fp_NewFrameRateLimit;
    }

   bool 
        RenderingManager::IsVSyncEnabled() 
        const noexcept
    {
        return pm_IsVSyncEnabled;
    }

    void 
        RenderingManager::SetVSync(const bool fp_IsEnabled)
        noexcept
    {
        pm_IsVSyncEnabled = fp_IsEnabled;
    }
}
