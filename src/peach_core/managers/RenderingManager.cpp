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

    PEACH_STATUS_CODE 
        RenderingManager::Initialize
        (
            const RendererType fp_RequiredRenderingBacked,
            shared_ptr<RenderingResourcePipe> fp_RenderingResourcePipe,
            SDL_Window* fp_MainWindow,
            const uint32_t fp_InitialWindowWidth,
            const uint32_t fp_InitialWindowHeight,
            const size_t fp_InitialFrameRate,
            const string& fp_LogOutputDirectory
        )   
    {
        if (pm_IsInitialized.load(std::memory_order_acquire)) //XXX: can log here since initializaiton only succeeds if rendering_logger gets created owo
        {
            rendering_logger->Warning("Tried to initialize rendering manager again >O<! what are you doing? this request was ignored", "RenderingManager");
            return PEACH_WARNING_TRIED_TO_INITIALIZE_RENDERING_MANAGER_AFTER_FULLY_INITIALIZED;
        }

        //////////////////// Set Frame Limit //////////////////// IMPORTANT: this has to be at the top otherwise the threads will start with garbage data here owo

        pm_CurrentFrameRateLimit = fp_InitialFrameRate;
        RENDER_FRAME_TIME_STEP = 1.0f / (float)fp_InitialFrameRate;

        //////////////////// Initialize Logger ////////////////////

        rendering_logger = LogManager::get_single().CreateUniqueLogger("RenderingManager", PEACH_LOGGER_DEFAULT_FLAGS, fp_LogOutputDirectory);

        if (not rendering_logger) [[unlikely]]
        {
            PEACH_PRINT_ERROR("RenderingManager failed to initialize the render_thread logger >w<");
            return PEACH_FATAL_FAILED_TO_CREATE_RENDERING_LOGGER;
        }

        rendering_logger->Info("RenderingLogger successfully initialized", "RenderingManager");

        //////////////////// Validate Window, and Store ////////////////////

        if(not fp_MainWindow) [[unlikely]] //don't needa check rendering_logger cause rendering manager can never be created without a valid logger instance owo
        {
            rendering_logger->Fatal("Tried to pass a nullptr reference to SDL_Window while starting Peach-E's RenderingManager owo", "RenderingManager");
            return PEACH_ERROR_NULLPTR_REF_PASSED;
        }

        //////////////////// Initialize Loading Queue ////////////////////

        if (not fp_RenderingResourcePipe)
        {
            rendering_logger->Fatal
            (
                "Initialization failed: RenderingManager was not able to obtain a valid LoadingQueue, exiting execution immediately, did you pass a nullptr reference to the audio resource transfer pipe?", 
                "RenderingManager"
            );
            
            return PEACH_FATAL_ERROR_PASSED_NULLPTR_REFERENCE_TO_RESOURCE_MANAGER_RENDERING_RESOURCE_PIPE;
        }

        pm_LoadedResourceQueue = fp_RenderingResourcePipe;

        rendering_logger->Info("RenderingManager successfully retrieved loaded rendering resource queue from ResourceManager", "RenderingManager");

        //////////////////// Initialize Rendering Backend ////////////////////

        #ifdef PEACH_RENDERER_OPENGL
            if(fp_RequiredRenderingBacked == RendererType::OpenGL)
            {
                ////Set Core Profile for OpenGL Context whatever the fuck that means
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

                //// Set OpenGL version
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

                SDL_GLContext f_OpenGLContext = SDL_GL_CreateContext(fp_MainWindow);

                if (not f_OpenGLContext)
                {
                    rendering_logger->Fatal(fmt::format("Failed to create OpenGL context: {}", SDL_GetError()), "OpenGL::Renderer");
                    return PEACH_ERROR_FAILED_TO_CREATE_GL_CONTEXT;
                }

                glewExperimental = GL_TRUE; //????????????????? wtf glew

                if (glewInit() != GLEW_OK)
                {
                    rendering_logger->Fatal("Failed to create GLEW context: " + static_cast<string>("OWO"), "RenderingManager");
                    return PEACH_ERROR_FAILED_INITIALIZE_GLEW;
                }

                rendering_logger->Info("GLEW initialized properly. Successfully initialized OpenGL!", "RenderingManager");

                pm_OpenGLRenderer = make_unique<OpenGL::Renderer>(); //needa make context current once the thread loop starts

                PEACH_STATUS_CODE result = pm_OpenGLRenderer->Initialize(fp_MainWindow, f_OpenGLContext, false, fp_LogOutputDirectory);

                if(result != PEACH_OK)
                {
                    rendering_logger->Fatal("Failed to initialize the OpenGL Renderer, engine is exiting execution immediately owo", "RenderingManager");
                    return result; //not sure if exit should be used here Futur ryan: no it really shouldn't uwu
                }
                
                pm_RenderThread = thread
                (
                    &RenderingManager::RenderLoopGL,
                    this
                );
            }
        #endif /*PEACH_RENDERER_OPENGL*/

        
        #ifdef PEACH_RENDERER_VULKAN
            if(fp_RequiredRenderingBacked == RendererType::Vulkan)
            {
                if (volkInitialize() != VK_SUCCESS)
                {
                    rendering_logger->Fatal("Volk failed to initialize! unable to find dynamic library to query vulkan driver for capabilities and functions", "RenderingManager");
                    return PEACH_ERROR_FAILED_TO_FIND_VULKAN_DYNAMIC_LIBRARY_FOR_FUNCTION_PFN_QUERYING;
                }

                pm_VulkanRenderer = make_unique<Vulkan::Renderer>();
                
                if (not pm_VulkanRenderer->Initialize(fp_MainWindow, fp_InitialWindowWidth, fp_InitialWindowHeight, fp_LogOutputDirectory))
                {
                    rendering_logger->Fatal("Failed to initialize Vulkan! ending program execution immediately", "RenderingManager");
                    return PEACH_ERROR_FAILED_TO_INITIALIZE_VULKAN;
                }

                rendering_logger->Info("Success! VulkanRenderer initialized properly, full rendering capabilities should be ready UwU", "RenderingManager");

                pm_RenderThread = thread
                (
                    &RenderingManager::RenderLoopVK,
                    this
                );
            }
        #endif /*PEACH_RENDERER_VULKAN*/

        #ifdef PEACH_RENDERER_METAL
            if(fp_RequiredRenderingBacked == RendererType::Metal)
            {
                pm_MetalRenderer = make_unique<Metal::Renderer>();

                if (pm_MetalRenderer->Initialize(pm_MainWindow, rendering_logger) != PEACH_OK)
                {
                    rendering_logger->Fatal("Initialization failed: RenderingManager was not able to create a valid Metal context, exiting execution immediately", "RenderingManager");
                    GameManager::get_single().ThreadPanicShutdown(PEACH_FATAL_ERROR_FAILED_TO_INITIALIZE_METAL); //not sure if exit should be used here
                    pm_IsRunning.store(false, std::memory_order_release);        
                }

                pm_RenderThread = thread
                (
                    &RenderingManager::RenderLoopMetal,
                    this
                );
            }
        #endif /*PEACH_RENDERER_METAL*/

            //rendering_logger->Fatal("Invalid rendering backend was selected, RenderingManager was not able to initialize properly", "RenderingManager");
        

        //////////////////// Initialization Successful! ////////////////////

        pm_IsInitialized.store(true); //owo

        return PEACH_OK;
    }
   
    void
        RenderingManager::ShutdownSubsystem(Logger*const logger)
    {
        if(not logger) [[unlikely]]
        {
            PEACH_PRINT_ERROR("Tried to pass nullptr reference to logger inside RenderingManager::ShutdownSubsystem()");
            return;
        }

        if(not pm_IsInitialized.load(std::memory_order_acquire))
        {
            logger->Error("Tried to call ShutdownSubsystem() on RenderingManager when render thread was never started owo wtf mang ;w;", "RenderingManager"); 
            return;
        }

        pm_IsRunning.store(false, std::memory_order_release);

        if (pm_RenderThread.joinable())
        {
            pm_RenderThread.join();                         // IMPORTANT: this will block until smth is returned so idk kinda can get fucked ig
            logger->Info("Successfully joined render thread", "RenderingManager");
        }
    }

   bool
        RenderingManager::ProcessCommands()
    {
        bool f_ContainsCommands = false;

        RenderCommand f_Command;
        while (pm_RenderCommandQueue.try_dequeue(f_Command))
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

#ifdef PEACH_RENDERER_OPENGL

    void
        RenderingManager::RenderLoopGL()
    {
        pm_OpenGLRenderer->UpdateForNewThread(); //XXX: makes gl context current on this thread, and updates logger thread owner >w< femboy hooters when ;w;

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

#endif

#ifdef PEACH_RENDERER_VULKAN

    void
        RenderingManager::RenderLoopVK()
    {
        pm_VulkanRenderer->UpdateForNewThread();

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

#endif

#ifdef PEACH_RENDERER_METAL

    void
        RenderingManager::RenderLoopMetal()
    {
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
}
