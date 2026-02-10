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
#include "RenderingManager.h"
/*
    This class is used to manage the render thread, and queue/unqueue objects safely
    Copyright(c) 2024-present Ranyodh Singh Mandur.

*/

namespace PeachCore {

    void 
        RenderingManager::Shutdown()
    {
#ifndef __APPLE__
        if (pm_OpenGLRenderer)
        {
            //SDL_DestroyWindow(pm_MainWindow);

            ////delete pm_MainWindow; //WARNING: DO NOT UNCOMMENT THIS, IT WILL CAUSE A HEAP MEMORY VIOLATION
            //pm_MainWindow = nullptr;
        }
#endif


        SDL_Quit(); //Render thread controls everything SDL related so if the render loop is exiting SDL should quit since no other thread touches or relies on SDL related functionality
    }

    [[nodiscard]] bool 
        RenderingManager::Initialize
        (
            const RendererType fp_DesiredRenderer,
            const string& fp_LogOutputDirectory
        )   
    {
        //////////////////// Initialize Logger ////////////////////

        rendering_logger = Logger::CreateShared("RenderingManager", PEACH_LOGGER_DEFAULT_FLAGS, fp_LogOutputDirectory);

        if (not rendering_logger)
        {
            PrintError("[CRITICAL_LOGGING_ERROR]: RenderingManager failed to initialize the render_thread logger >w<");
            return false;
        }

        rendering_logger->Debug("RenderingLogger successfully initialized", "RenderingManager");

        //////////////////// Intialize InputManager ////////////////////

        if (not InputManager::get_single().Initialize(fp_LogOutputDirectory, PEACH_LOGGER_DEFAULT_FLAGS))
        {

            return false;
        }

        //////////////////// Initialize Loading and Command Queues ////////////////////

        if (not SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) //YEAH THIS should be here oops idk how we created a SDL window before calling init oop
        {
            rendering_logger->Fatal(format("SDL could not initialize! ending engine program execution immediately, SDL_Error: {}", string(SDL_GetError())), "GameManager");
            return false;
        }
        else if (not InitializeLoadingQueue())
        {
            rendering_logger->Fatal("Initialization failed: RenderingManager was not able to obtain a valid LoadingQueue, exiting execution immediately", "RenderingManager");
            return false;
        }

        InitializeDrawCommandQueue();

        //////////////////// Initialize Rendering Backend ////////////////////

        if (fp_DesiredRenderer == RendererType::Vulkan)
        {
            if (not InitializeVulkan())
            {
                rendering_logger->Fatal("Initialization failed: RenderingManager was not able to initialize Vulkan, exiting execution immediately", "RenderingManager");
                exit(PEACH_ERROR_FAILED_TO_INITIALIZE_VULKAN);
            }
        }
        #ifndef __APPLE__
            else if(fp_DesiredRenderer == RendererType::OpenGL)
            {
                if (InitializeOpenGL() != PEACH_OK)
                {
                    rendering_logger->Fatal("Initialization failed: RenderingManager was not able to create a valid OpenGL context, exiting execution immediately", "RenderingManager");
                    exit(PEACH_ERROR_FAILED_TO_INITIALIZE_OPENGL); //not sure if exit should be used here
                }
            }
        #endif
        else
        {
            rendering_logger->Fatal("Invalid rendering backend was selected, RenderingManager was not able to initialize properly", "RenderingManager");
        }

        pm_IsInitialized = true;

        return true;
    }

    [[nodiscard]] unique_ptr<unsigned char>
        RenderingManager::LoadDefaultTexture()
    {
        int f_Width = 0, f_Height = 0, f_Channels = 0;

        unique_ptr<unsigned char> f_Pixels
        (
            stbi_load_from_memory
            (
                NullResources::PEACH_NULL_TEXTURE,
                static_cast<int>(NullResources::GetDefaultTextureSize()),
                &f_Width,
                &f_Height,
                &f_Channels,
                4 // force RGBA
            )
        );

        if (not f_Pixels)
        {
            PrintError(format("Failed to load texture default texture! (wtf), reason: {}", stbi_failure_reason()));
            return nullptr;
        }

        return move(f_Pixels);
    }

    void
        RenderingManager::RenderLoopVK
        (
            const string& fp_LogOutputDirectory,
            latch& fp_InitLatch
        )
    {
        if (not Initialize(RendererType::Vulkan, fp_LogOutputDirectory))
        {

            return;
        }

        fp_InitLatch.count_down();

        while (pm_IsRunning.load(std::memory_order_acquire))
        {
            // Block until main thread wakes us
            pm_RenderSemaphore.acquire();

            if (not pm_IsRunning.load(std::memory_order_acquire))
            {
                break; // Double check after wake
            }

            ProcessCommands();
            PresentFrameVK(); // swap buffers etc.
            PollUserInputEvents();
        }

        Shutdown();
    }

    void
        RenderingManager::RenderLoopGL
        (
            const string& fp_LogOutputDirectory,
            latch& fp_InitLatch
        )
    {
        if (not Initialize(RendererType::OpenGL, fp_LogOutputDirectory))
        {

            return;
        }

        fp_InitLatch.count_down();

        while (pm_IsRunning.load(std::memory_order_acquire))
        {
            // Block until main thread wakes us
            pm_RenderSemaphore.acquire();

            if (not pm_IsRunning.load(std::memory_order_acquire))
            {
                break; // Double check after wake
            }

            ProcessCommands();
            PresentFrameGL(); // swap buffers etc.
            PollUserInputEvents();
        }

        Shutdown();
    }

    void
        RenderingManager::RequestRender()
    {
        pm_RenderSemaphore.release(); // Gives 1 ticket, wakes render thread
    }

    void
        RenderingManager::Stop()
    {
        pm_IsRunning.store(false, std::memory_order_release);
        pm_RenderSemaphore.release(); // Wake it up to exit        
    }

    void
        RenderingManager::PollUserInputEvents()
    {
        InputManager::get_single().PollEvents();

        InputManager::get_single().GetWindowCloseRequests(pm_CloseWindowRequests);

        for (const auto& lv_Window : pm_CloseWindowRequests)
        {
            if (SDL_GetWindowID(pm_MainWindow) == lv_Window)
            {
                pm_IsRunning.store(false, std::memory_order_release);
                //pm_VulkanRenderer->CleanUp();
            }

            SDL_DestroyWindow(SDL_GetWindowFromID(lv_Window)); //WARNING DO NOT CLOSE WINDOW HERE SEND A REQUEST TO THE RENDERING MANAGER FOR THAT
        } 

        glm::vec2 f_MousePos = InputManager::get_single().GetCurrentMousePosition();

        Print(format("mouse x : {}, y: {}", f_MousePos.x, f_MousePos.y), Colours::Green);
    }

    [[nodiscard]] bool
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
                PrintError("invalid opcode found for rendering manager! WHAT ARE YE DOIN SON?!?!", Colours::BrightRed);
            }
        }

        return f_ContainsCommands;
    }

    [[nodiscard]] bool
        RenderingManager::PresentFrameVK() //just assuming vulkan for now but this is where the backend magic happens
    {
        //////////////////// Submit Draw Calls ////////////////////

        uint32_t f_StatusCode = pm_VulkanRenderer->BeginFrame();
        if (f_StatusCode & ~Vulkan::Renderer::StatusCode::OK) //don't even try to draw into cmd buffer or end frame is frame didnt start properly
        {
            PrintError(format("BeginFrame() failed exit, StatusCode: {}", f_StatusCode), Colours::BrightMagenta);
            return false;
        }

        f_StatusCode = pm_VulkanRenderer->DrawFrame();
        if (f_StatusCode & ~Vulkan::Renderer::StatusCode::OK)
        {
            PrintError(format("DrawFrame() failed exit, StatusCode: {}", f_StatusCode), Colours::BrightMagenta);
            return false;
        }

        f_StatusCode = pm_VulkanRenderer->EndFrame();
        if (f_StatusCode & ~Vulkan::Renderer::StatusCode::OK)
        {
            PrintError(format("EndFrame() failed exit, StatusCode: {}", f_StatusCode), Colours::BrightMagenta);
            return false;
        }

        return true;
    }

    [[nodiscard]] bool
        RenderingManager::PresentFrameGL()
    {

        return true;
    }

    [[nodiscard]] bool
        RenderingManager::CreateSDLWindow
        (
            SDL_Window** fp_SDLWindow,
            const RendererType fp_RenderingBackend,
            const string& fp_WindowTitle,
            const unsigned int fp_WindowWidth,
            const unsigned int fp_WindowHeight
        )
    {
        if (*fp_SDLWindow)
        {
            rendering_logger->Error("Tried passing a valid SDL_Window* handle for window creation, please cleanup original SDL window or dereference pointer before attempting to create a new SDL window", "RenderingManager");
            return false;
        }

        uint64_t f_WindowFlags = 1;

        if (fp_RenderingBackend == RendererType::OpenGL)
        {
            f_WindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
        }
        else if (fp_RenderingBackend == RendererType::Vulkan)
        {
            f_WindowFlags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE;
        }
        else
        {
            rendering_logger->Error("Invalid Renderer Type was passed to CreateSDLWindow(), please pass a valid rendering backend type", "RenderingManager");
            return false;
        }

        *fp_SDLWindow = SDL_CreateWindow
        (
            fp_WindowTitle.c_str(),
            fp_WindowWidth,
            fp_WindowHeight,
            f_WindowFlags
        );

        if (not *fp_SDLWindow)
        {
            rendering_logger->Fatal("Window could not be created! SDL_Error: " + string(SDL_GetError()), "RenderingManager");
            return false;
        }

        SDL_WindowID f_WindowID = SDL_GetWindowID(*fp_SDLWindow);
        pm_CurrentlyActiveWindows[f_WindowID] = *fp_SDLWindow;

        return true;
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

    [[nodiscard]] shared_ptr<RenderCommandPipe>
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

    #ifndef __APPLE__
        bool
            RenderingManager::CreateOpenGLRenderer
            (
                SDL_Window* fp_Window
            )
        {
            if (not fp_Window)
            {
                rendering_logger->Warning("Please try creating an SDL window before trying to create a PeachRenderer!", "RenderingManager");
                return false;
            }

            if (pm_OpenGLRenderer.get())
            {
                pm_OpenGLRenderer.reset(nullptr);
            }

            pm_OpenGLRenderer = make_unique<OpenGL::Renderer>(fp_Window, rendering_logger);
            return true;
        }

        void
            RenderingManager::DestroyOpenGLRenderer()
        {
            pm_OpenGLRenderer.reset(nullptr);
        }

        PEACH_STATUS_CODE
            RenderingManager::InitializeOpenGL()
        {
            if (pm_IsInitialized)
            {
                rendering_logger->Warning("RenderingManager tried to initialize OpenGL when rendering has already been initialized", "RenderingManager");
                return PEACH_ERROR_FAILED_TO_INITIALIZE_OPENGL;
            }

            if (not CreateSDLWindow(&pm_MainWindow, RendererType::OpenGL, "Peach Window", 800, 600))
            {
                rendering_logger->Fatal("Initialization failed: RenderingManager was not able to create the main window, exiting execution immediately", "RenderingManager");
                return PEACH_ERROR_FAILED_TO_CREATE_MAIN_WINDOW;
            }

            rendering_logger->Debug("main SDL window successfully created", "RenderingManager");

            pm_OpenGLRenderer = make_unique<OpenGL::Renderer>(pm_MainWindow, rendering_logger, true);

            if (glewInit() != GLEW_OK)
            {
                rendering_logger->Fatal("Failed to create GLEW context: " + static_cast<string>("OWO"), "RenderingManager");
                SDL_DestroyWindow(pm_OpenGLRenderer->GetMainWindow());
                return PEACH_ERROR_FAILED_INITIALIZE_GLEW;
            }

            rendering_logger->Debug("GLEW initialized properly", "RenderingManager");

            rendering_logger->Info("Successfully initialized OpenGL!", "RenderingManager");

            return PEACH_OK;
        }

        [[nodiscard]] OpenGL::Renderer*
            RenderingManager::GetOpenGLRenderer()
        {
            return pm_OpenGLRenderer.get();
        }
    #endif

    bool
        RenderingManager::InitializeVulkan()
    {
        //Manually load libvulkan.1.dylib since volk cant find it w the regular init method, and run volkInitializeCustom and pass the proc pointer
        #ifdef __APPLE__ //fuck u tim apple we won REST IN PISS BOZO

            void* f_VulkanDylib = dlopen("@executable_path/../Frameworks/libvulkan.1.dylib", RTLD_NOW | RTLD_LOCAL);

            if (not f_VulkanDylib) 
            {
                rendering_logger->PEACH_LOG(format("Couldn't load libvulkan.1.dylib with Error: {}", dlerror()), "RenderingManager");
                return false;
            }

            auto f_GetProcAddress = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(f_VulkanDylib, "vkGetInstanceProcAddr"));
            
            if (not f_GetProcAddress)
            {
                rendering_logger->PEACH_LOG(format("Couldn't find symbol: 'vkGetInstanceProcAddr' with Error: {}", dlerror()), "RenderingManager");
                return false;
            }
            
            volkInitializeCustom(f_GetProcAddress);  //custom initialize since volk cant find the libvulkan inside the Frameworks part of the bundle

        #else ///Used for everything that isnt dumb fuck tim apple

            if (volkInitialize() != VK_SUCCESS)
            {
                rendering_logger->Fatal("Volk failed to initialize! ending program execution immediately", "RenderingManager");
                return false;
            }
        
        #endif

        if (not CreateSDLWindow(&pm_MainWindow, RendererType::Vulkan, "Peach Window", 800, 600))
        {
            rendering_logger->Fatal("Initialization failed: RenderingManager was not able to create the main window, exiting execution immediately", "RenderingManager");
            exit(PEACH_ERROR_FAILED_TO_CREATE_MAIN_WINDOW); //idk if i wanna exit here but it doesn really matter, i might want the "stack trace" from the false chain created by intialize failing
        }   

        pm_VulkanRenderer = make_unique<Vulkan::Renderer>();
        
        if (not pm_VulkanRenderer->Initialize(pm_MainWindow, rendering_logger))
        {
            rendering_logger->Fatal("Failed to initialize Vulkan! ending program execution immediately", "RenderingManager");
            return false;
        }

        rendering_logger->Info("Success! VulkanRenderer initialized properly, full rendering capabilities should be ready UwU", "RenderingManager");

        return true; // >w<
    }

    void 
        RenderingManager::ResizeWindow()
    {

    }

    void
        RenderingManager::GetCurrentViewPort()
    {
      
    }

    [[nodiscard]] uint64_t
        RenderingManager::GetFrameRateLimit() 
        const noexcept
    {
        return pm_FrameRateLimit;
    }

    [[nodiscard]] bool 
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

    void 
        RenderingManager::SetFrameRateLimit(uint64_t fp_Limit) 
        noexcept
    {
        pm_FrameRateLimit = fp_Limit;
    }
}
