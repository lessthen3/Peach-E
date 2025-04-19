#include "../../include/Peach-Core/Managers/RenderingManager.h"
/*
	This class is used to manage the render thread, and queue/unqueue objects safely
	Copyright(c) 2024-present Ranyodh Singh Mandur.

*/

namespace PeachCore {


    RenderingManager::~RenderingManager() 
    {
        if (pm_Renderer)
        {
            pm_Renderer.reset(nullptr);
        }
    }

    void 
        RenderingManager::Shutdown()
    {
        if (pm_Renderer)
        {
            //SDL_DestroyWindow(pm_MainWindow);

            ////delete pm_MainWindow; //WARNING: DO NOT UNCOMMENT THIS, IT WILL CAUSE A HEAP MEMORY VIOLATION
            //pm_MainWindow = nullptr;
        }
    }

    bool 
        RenderingManager::Initialize
        (
            const RendererType fp_DesiredRenderer,
            const string& fp_LogOutputDirectory,
            shared_ptr<Console> fp_Console
        )   
    {
        if (not fp_Console.get())
        {
            PrintError("Tried to initialize RenderingManager with a nullptr reference to the Console");
            return false;
        }

        rendering_logger = make_shared<LogManager>(); 
        rendering_logger->Initialize("render_thread", fp_LogOutputDirectory, "RenderingManager", fp_Console);
        rendering_logger->LogAndPrint("RenderingLogger successfully initialized", "RenderingManager", LogManager::LogLevel::Debug);

        if(fp_DesiredRenderer == RendererType::OpenGL)
        {
            if (not InitializeOpenGL())
            {
                rendering_logger->LogAndPrint("Initialization failed: RenderingManager was not able to create a valid OpenGL context, exiting execution immediately", "RenderingManager", PeachCore::LogManager::LogLevel::Fatal);
                exit(FAILED_TO_INITIALIZE_OPENGL); //not sure if exit should be used here
            }
        }
        else if (fp_DesiredRenderer == RendererType::Vulkan)
        {
            if (not InitializeVulkan())
            {
                rendering_logger->LogAndPrint("Initialization failed: RenderingManager was not able to initialize Vulkan, exiting execution immediately", "RenderingManager", PeachCore::LogManager::LogLevel::Fatal);
                exit(FAILED_TO_INITIALIZE_VULKAN);
            }
        }

        //InitializeQueues(); IM NOT SURE IF THIS SHOULD BE HERE AHHHHHHHHH WHY DID I REMAKE THE LOGGING SYSTEM I SHOULDA HIT A COMMIT THEN DID THIS AHHHH IM RETARDED

        pm_IsInitialized = true;
        return true;
    }

    bool
        RenderingManager::CreateSDLWindow
        (
            SDL_Window** fp_SDLWindow,
            const RendererType fp_RenderingBackend,
            const string& fp_WindowTitle,
            const unsigned int fp_WindowWidth,
            const unsigned int fp_WindowHeight
        )
        const
    {
        if (*fp_SDLWindow)
        {
            rendering_logger->LogAndPrint("Tried passing a valid SDL_Window* handle for window creation, please cleanup original SDL window or dereference pointer before attempting to create a new SDL window", "RenderingManager", LogManager::LogLevel::Error);
            return false;
        }

        uint64_t f_WindowFlags = -1;

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
            rendering_logger->LogAndPrint("Invalid Renderer Type was passed to CreateSDLWindow(), please pass a valid rendering backend type", "RenderingManager", LogManager::LogLevel::Error);
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
            rendering_logger->LogAndPrint("Window could not be created! SDL_Error: " + string(SDL_GetError()), "RenderingManager", LogManager::LogLevel::Fatal);
            return false;
        }

        return true;
    }

    bool
        RenderingManager::CreatePeachRenderer
        (
            SDL_Window* fp_Window
        )
    {
        if (not fp_Window)
        {
            rendering_logger->LogAndPrint("Please try creating an SDL window before trying to create a PeachRenderer!", "RenderingManager", LogManager::LogLevel::Warning);
            return false;
        }

        if (pm_Renderer.get())
        {
            pm_Renderer.reset(nullptr);
        }

        pm_Renderer = make_unique<PeachRenderer>(fp_Window, rendering_logger);
        return true;
    }

    void
        RenderingManager::DestroyPeachRenderer()
    {
        pm_Renderer.reset(nullptr);
    }

    //creates a window and opengl context, enables sfml 2d graphics and such as well, returns the command queue for thread safe control
    [[nodiscard]] shared_ptr<CommandQueue> 
        RenderingManager::InitializeQueues()
    {
        if (pm_CommandQueue || pm_LoadedResourceQueue)
        {
            //rendering_logger->LogAndPrint("RenderingManager already initialized.", "RenderingManager", LogManager::LogLevel::Warning);
            return nullptr;
        }

        pm_CommandQueue = make_shared<CommandQueue>();
        pm_LoadedResourceQueue = ResourceLoadingManager::ResourceLoader().GetDrawableResourceLoadingQueue();

        //rendering_logger->LogAndPrint("RenderingManager successfully initialized >w<", "RenderingManager", LogManager::LogLevel::Debug);

        pm_AreQueuesInitialized = true;

        return pm_CommandQueue; //returns one and only one ptr to whoever initializes RenderingManager, this is meant only for the main thread
    }

    bool
        RenderingManager::InitializeOpenGL()
    {
        if (pm_IsRenderingInitialized)
        {
            rendering_logger->LogAndPrint("RenderingManager tried to initialize OpenGL when rendering has already been initialized", "RenderingManager", LogManager::LogLevel::Warning);
            return false;
        }

        if (not pm_AreQueuesInitialized)
        {
            rendering_logger->LogAndPrint("RenderingManager tried to initialize OpenGL before initializing command/loading queues!", "RenderingManager", LogManager::LogLevel::Error);
            return false;
        }

        if (not CreateSDLWindow(&pm_MainWindow, RendererType::OpenGL, "Peach Window", 800, 600))
        {
            rendering_logger->LogAndPrint("Initialization failed: RenderingManager was not able to create the main window, exiting execution immediately", "RenderingManager", LogManager::LogLevel::Fatal);
            exit(FAILED_TO_CREATE_MAIN_WINDOW);
        }

        rendering_logger->LogAndPrint("main SDL window successfully created", "RenderingManager", PeachCore::LogManager::LogLevel::Debug);

        pm_Renderer = make_unique<PeachRenderer>(pm_MainWindow, rendering_logger, true);

        if (glewInit() != GLEW_OK)
        {
            rendering_logger->LogAndPrint("Failed to create GLEW context: " + static_cast<string>("OWO"), "RenderingManager", LogManager::LogLevel::Fatal);
            SDL_DestroyWindow(pm_Renderer->GetMainWindow());
            return false;
        }

        rendering_logger->LogAndPrint("GLEW initialized properly", "RenderingManager", LogManager::LogLevel::Debug);

        rendering_logger->LogAndPrint("Peach Editor successfully initialized OpenGL", "RenderingManager", PeachCore::LogManager::LogLevel::Debug);

        pm_IsRenderingInitialized = true;
        return true;
    }

    bool
        RenderingManager::InitializeVulkan()
    {
        if (volkInitialize() != VK_SUCCESS)
        {
            rendering_logger->LogAndPrint("Volk failed to initialize! ending program execution immediately", "RenderingManager", PeachCore::LogManager::LogLevel::Fatal);
            return false;
        }

        vkb::InstanceBuilder builder;

        return true; // >w<
    }

    void 
        RenderingManager::ProcessCommands() 
    {
        DrawCommand f_DrawCommand;
        while (pm_CommandQueue->PopSendersQueue(f_DrawCommand))
        {
            for (auto& drawable_data : f_DrawCommand.DrawableData)
            {
                visit(overloaded
                    {
                    [&](const vector<CreateDrawableData>& fp_Data)
                    {
                        // Handle creation logic here
                    },
                    [&](const vector<UpdateActiveDrawableData>& fp_Data)
                    {
                        // Handle update logic here
                        // This could involve updating position based on deltaPosition
                        // Setting visibility, layer sorting, etc.
                    },
                    [&](const vector<DeleteDrawableData>& fp_Data)
                    {
                        // Handle deletion logic here
                        // Ensure resources are properly released and objects are cleaned up
                    }
                    }, drawable_data);
            }
        }
    }

    void 
        RenderingManager::ProcessLoadedResourcePackages()
    {
        unique_ptr<LoadedResourcePackage> ResourcePackage;

        while (pm_LoadedResourceQueue->PopLoadedResourceQueue(ResourcePackage)) 
        {
            visit(overloaded
                {
                [&](TextureData& fp_RawByteData)
                {
                    // Handle creation logic here
                },
                [](auto&&)
                {
                    //THIS DOESN'T WORK AND IDK Y LAMBDA SMTH IDK FUCK IT ill come back to it later
                    // Default handler for any unhandled types
                    //rendering_logger->LogAndPrint("Unhandled type in variant for ProcessLoadedResourcePackage", "RenderingManager", LogManager::LogLevel::Warning);
                }
                }, ResourcePackage.get()->ResourceData);
        }
    }

    void 
        RenderingManager::RenderFrame(bool fp_IsStressTest)
    {
        if (not pm_IsRenderingInitialized)
        {
            rendering_logger->LogAndPrint("Please initialize RenderingManager before trying to render anything!", "RenderingManager", LogManager::LogLevel::Warning);
            return;
        }

        if (not pm_Renderer->GetMainWindow())
        {
            rendering_logger->LogAndPrint("Please assign a valid SDL window to pm_MainWindow before trying to render!", "RenderingManager", LogManager::LogLevel::Warning);
            return;
        }

        //ProcessLoadedResourcePackages(); //move all loaded objects into memory here if necessary
        //ProcessCommands(); //process all updates

        bool f_IsGameRuntimeOver = false;

        // Main loop that continues until the window is closed
        while (not f_IsGameRuntimeOver)
        {
            if (pm_IsShutDown) //used to stop rendering loop if possible when ForceQuit() is called
            {
                //glClear(GL_COLOR_BUFFER_BIT);
                pm_IsShutDown = false; //gotta reset it otherwise everytime we run the scene again it just closes immediately lmao
                break;
            }
            SDL_Event event;

            while (SDL_PollEvent(&event))
            {
                //ImGui_ImplSDL2_ProcessEvent(&event);

                if (event.type == SDL_EVENT_QUIT)
                {
                    f_IsGameRuntimeOver = true;
                }
            }

            //if (fp_IsStressTest) //used to stop rendering loop after one cycle for testing
            //{
            //    pm_IsShutDown = false; //gotta reset it otherwise everytime we run the scene again it just closes immediately lmao
            //    break;
            //}
        }

        Shutdown(); //cleanup everything here
    }

    void 
        RenderingManager::ResizeWindow()
    {

    }

    void
        RenderingManager::GetCurrentViewPort()
    {
      
    }

    [[nodiscard]] unsigned int
        RenderingManager::GetFrameRateLimit() 
        const
    {
        return pm_FrameRateLimit;
    }

    bool 
        RenderingManager::IsVSyncEnabled() 
        const
    {
        return pm_IsVSyncEnabled;
    }

    void 
        RenderingManager::SetVSync(const bool fp_IsEnabled)
    {
        if (fp_IsEnabled)
        {
            pm_IsVSyncEnabled = fp_IsEnabled;
        }
    }

    void 
        RenderingManager::SetFrameRateLimit(unsigned int fp_Limit)
    {
        pm_FrameRateLimit = fp_Limit;
    }

    [[nodiscard]] PeachRenderer*
        RenderingManager::GetPeachRenderer()
    {
        return pm_Renderer.get();
    }
}