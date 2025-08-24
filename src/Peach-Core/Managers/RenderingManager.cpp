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
#include "../../include/Peach-Core/Managers/RenderingManager.h"
/*
    This class is used to manage the render thread, and queue/unqueue objects safely
    Copyright(c) 2024-present Ranyodh Singh Mandur.

*/

namespace PeachCore {


    RenderingManager::~RenderingManager() 
    {
        #ifndef __APPLE__
            if (pm_OpenGLRenderer)
            {
                pm_OpenGLRenderer.reset(nullptr);
            }
        #endif
    }

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
    }

    bool 
        RenderingManager::Initialize
        (
            const RendererType fp_DesiredRenderer,
            const string& fp_LogOutputDirectory,
            shared_ptr<Console> fp_Console
        )   
    {
        //////////////////// Nullptr check for PeachConsole ref ////////////////////

        if (not fp_Console)
        {
            PrintError("Tried to initialize RenderingManager with a nullptr reference to the Console");
            return false;
        }

        //////////////////// Initialize Logger ////////////////////

        rendering_logger = make_shared<LogManager>(); 
        rendering_logger->Initialize(ThreadName::RenderThread, fp_LogOutputDirectory, "RenderingManager", fp_Console, LogManager::LogLevel::All);
        rendering_logger->PEACH_LOG("RenderingLogger successfully initialized", "RenderingManager", LogManager::LogLevel::Debug);

        //////////////////// Initialize Loading and Command Queues ////////////////////

        if (not InitializeLoadingQueue())
        {
            rendering_logger->PEACH_LOG("Initialization failed: RenderingManager was not able to obtain a valid LoadingQueue, exiting execution immediately", "RenderingManager", PeachCore::LogManager::LogLevel::Fatal);
            return false;
        }

        InitializeDrawCommandQueue();

        //////////////////// Initialize Rendering Backend ////////////////////

        if (fp_DesiredRenderer == RendererType::Vulkan)
        {
            if (not InitializeVulkan())
            {
                rendering_logger->PEACH_LOG("Initialization failed: RenderingManager was not able to initialize Vulkan, exiting execution immediately", "RenderingManager", PeachCore::LogManager::LogLevel::Fatal);
                exit(FAILED_TO_INITIALIZE_VULKAN);
            }
        }
        #ifndef __APPLE__
            else if(fp_DesiredRenderer == RendererType::OpenGL)
            {
                if (not InitializeOpenGL())
                {
                    rendering_logger->PEACH_LOG("Initialization failed: RenderingManager was not able to create a valid OpenGL context, exiting execution immediately", "RenderingManager", PeachCore::LogManager::LogLevel::Fatal);
                    exit(FAILED_TO_INITIALIZE_OPENGL); //not sure if exit should be used here
                }
            }
        #endif
        else
        {
            rendering_logger->PEACH_LOG("Invalid rendering backend was selected, RenderingManager was not able to initialize properly", "RenderingManager", LogManager::LogLevel::Fatal);
        }

        pm_IsInitialized = true;

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
            rendering_logger->PEACH_LOG("Tried passing a valid SDL_Window* handle for window creation, please cleanup original SDL window or dereference pointer before attempting to create a new SDL window", "RenderingManager", LogManager::LogLevel::Error);
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
            rendering_logger->PEACH_LOG("Invalid Renderer Type was passed to CreateSDLWindow(), please pass a valid rendering backend type", "RenderingManager", LogManager::LogLevel::Error);
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
            rendering_logger->PEACH_LOG("Window could not be created! SDL_Error: " + string(SDL_GetError()), "RenderingManager", LogManager::LogLevel::Fatal);
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
            rendering_logger->PEACH_LOG("RenderingManager already retrieved the loaded resource queue from ResourceManager >O<", "RenderingManager", LogManager::LogLevel::Warning);
            return false;
        }

        pm_LoadedResourceQueue = ResourceManager::get_single().GetDrawableResourceLoadingQueue();

        if (not pm_LoadedResourceQueue)
        {
            rendering_logger->PEACH_LOG("RenderingManager failed to retrieve LoadingQueue from ResourceManager, nullptr ref was found >O<", "RenderingManager", LogManager::LogLevel::Error);
            return false;
        }

        rendering_logger->PEACH_LOG("RenderingManager successfully retrieved loaded resource queue from ResourceManager", "RenderingManager", LogManager::LogLevel::Info);

        return true; //returns one and only one ptr to whoever initializes RenderingManager, this is meant only for the main thread
    }

    bool
        RenderingManager::InitializeDrawCommandQueue()
    {
        if (pm_DrawCommandQueue)
        {
            rendering_logger->PEACH_LOG("RenderingManager already initialized the draw command queue >O<", "RenderingManager", LogManager::LogLevel::Warning);
            return false;
        }

        pm_DrawCommandQueue = make_shared<CommandQueue>();

        rendering_logger->PEACH_LOG("RenderingManager successfully initialized the draw command queue", "RenderingManager", LogManager::LogLevel::Info);

        return true; //returns one and only one ptr to whoever initializes RenderingManager, this is meant only for the main thread
    }

    [[nodiscard]] shared_ptr<CommandQueue>
        RenderingManager::GetDrawCommandQueue()
    {
        if (not pm_IsInitialized)
        {
            rendering_logger->PEACH_LOG("Attempted to get a reference to RenderingManager's DrawCommandQueue before RenderingManager was initialized, please initialize RenderingManager first UwU", "RenderingManager", LogManager::LogLevel::Error);
            return nullptr;
        }
        else if (pm_DrawCommandQueue.use_count() >= 2)
        {
            rendering_logger->PEACH_LOG("RenderingManager has already issued a reference to the draw command queue, fuck off", "RenderingManager", LogManager::LogLevel::Warning);
            return nullptr;
        }
        
        return pm_DrawCommandQueue;
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
                rendering_logger->PEACH_LOG("Please try creating an SDL window before trying to create a PeachRenderer!", "RenderingManager", LogManager::LogLevel::Warning);
                return false;
            }

            if (pm_OpenGLRenderer.get())
            {
                pm_OpenGLRenderer.reset(nullptr);
            }

            pm_OpenGLRenderer = make_unique<OpenGLRenderer>(fp_Window, rendering_logger);
            return true;
        }

        void
            RenderingManager::DestroyOpenGLRenderer()
        {
            pm_OpenGLRenderer.reset(nullptr);
        }

        bool
            RenderingManager::InitializeOpenGL()
        {
            if (pm_IsInitialized)
            {
                rendering_logger->PEACH_LOG("RenderingManager tried to initialize OpenGL when rendering has already been initialized", "RenderingManager", LogManager::LogLevel::Warning);
                return false;
            }

            if (not CreateSDLWindow(&pm_MainWindow, RendererType::OpenGL, "Peach Window", 800, 600))
            {
                rendering_logger->PEACH_LOG("Initialization failed: RenderingManager was not able to create the main window, exiting execution immediately", "RenderingManager", LogManager::LogLevel::Fatal);
                exit(FAILED_TO_CREATE_MAIN_WINDOW);
            }

            rendering_logger->PEACH_LOG("main SDL window successfully created", "RenderingManager", PeachCore::LogManager::LogLevel::Debug);

            pm_OpenGLRenderer = make_unique<OpenGLRenderer>(pm_MainWindow, rendering_logger, true);

            if (glewInit() != GLEW_OK)
            {
                rendering_logger->PEACH_LOG("Failed to create GLEW context: " + static_cast<string>("OWO"), "RenderingManager", LogManager::LogLevel::Fatal);
                SDL_DestroyWindow(pm_OpenGLRenderer->GetMainWindow());
                return false;
            }

            rendering_logger->PEACH_LOG("GLEW initialized properly", "RenderingManager", LogManager::LogLevel::Debug);

            rendering_logger->PEACH_LOG("Peach Editor successfully initialized OpenGL", "RenderingManager", PeachCore::LogManager::LogLevel::Debug);

            return true;
        }

        [[nodiscard]] OpenGLRenderer*
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
                rendering_logger->PEACH_LOG(format("Couldn't load libvulkan.1.dylib with Error: {}", dlerror()), "RenderingManager", PeachCore::LogManager::LogLevel::Fatal);
                return false;
            }

            auto f_GetProcAddress = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(f_VulkanDylib, "vkGetInstanceProcAddr"));
            
            if (not f_GetProcAddress)
            {
                rendering_logger->PEACH_LOG(format("Couldn't find symbol: 'vkGetInstanceProcAddr' with Error: {}", dlerror()), "RenderingManager", PeachCore::LogManager::LogLevel::Fatal);
                return false;
            }
            
            volkInitializeCustom(f_GetProcAddress);  //custom initialize since volk cant find the libvulkan inside the Frameworks part of the bundle

        #else ///Used for everything that isnt dumb fuck tim apple

            if (volkInitialize() != VK_SUCCESS)
            {
                rendering_logger->PEACH_LOG("Volk failed to initialize! ending program execution immediately", "RenderingManager", PeachCore::LogManager::LogLevel::Fatal);
                return false;
            }
        
        #endif

        if (not CreateSDLWindow(&pm_MainWindow, RendererType::Vulkan, "Peach Window", 800, 600))
        {
            rendering_logger->PEACH_LOG("Initialization failed: RenderingManager was not able to create the main window, exiting execution immediately", "RenderingManager", LogManager::LogLevel::Fatal);
            exit(FAILED_TO_CREATE_MAIN_WINDOW); //idk if i wanna exit here but it doesn really matter, i might want the "stack trace" from the false chain created by intialize failing
        }   

        string f_BaseDir = PHYSFS_getWriteDir(); //WARNING: USED ONLY FOR TESTING NEED THIS TO BE IN RESOURCEMANAGER

        ShaderUtils::BakedPipelineData f_BakedPipelineData;

        ShaderUtils::BakePipelineData(f_BaseDir + "/tests/first_triangle/triangle.vert.spv", f_BaseDir + "/tests/first_triangle/triangle.frag.spv", f_BakedPipelineData, rendering_logger.get());

        pm_VulkanRenderer = make_unique<VulkanRenderer>();
        
        if (not pm_VulkanRenderer->Initialize(pm_MainWindow, f_BakedPipelineData, rendering_logger))
        {
            rendering_logger->PEACH_LOG("Failed to initialize Vulkan! ending program execution immediately", "RenderingManager", PeachCore::LogManager::LogLevel::Fatal);
            return false;
        }

        rendering_logger->PEACH_LOG("Success! VulkanRenderer initialized properly, full rendering capabilities should be ready UwU", "RenderingManager", PeachCore::LogManager::LogLevel::Info);

        return true; // >w<
    }

    void 
        RenderingManager::RenderFrame()
    {
        if (not pm_IsInitialized)
        {
            rendering_logger->PEACH_LOG("Please initialize RenderingManager before trying to render anything!", "RenderingManager", LogManager::LogLevel::Warning);
            return;
        }
        //WARNING ARTIFACT FROM WHEN OPENGL WAS THE ONLY SUPPORTED RENDERER
        // if (not pm_OpenGLRenderer->GetMainWindow())
        // {
        //     rendering_logger->PEACH_LOG("Please assign a valid SDL window to pm_MainWindow before trying to render!", "RenderingManager", LogManager::LogLevel::Warning);
        //     return;
        // }

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

            #ifdef PEACH_RENDER_STRESS_TEST
                if (fp_IsStressTest) //used to stop rendering loop after one cycle for testing
                {
                    pm_IsShutDown = false; //gotta reset it otherwise everytime we run the scene again it just closes immediately lmao
                    break;
                }
            #endif

        }
    }

    void
        RenderingManager::ProcessDrawCommands()
    {
        DrawCommand f_DrawCommand;
        while (pm_DrawCommandQueue->PopSendersQueue(f_DrawCommand))
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
        //LoadedResourcePackage ResourcePackage;

        //while (pm_LoadedResourceQueue->PopLoadedResourceQueue(ResourcePackage)) 
        //{
        //    //visit(overloaded
        //    //    {
        //    //    [&](unique_ptr<TextureData> fp_RawByteData)
        //    //    {
        //    //        // Handle creation logic here
        //    //    },
        //    //    [](auto&&)
        //    //    {
        //    //        //THIS DOESN'T WORK AND IDK Y LAMBDA SMTH IDK FUCK IT ill come back to it later
        //    //        // Default handler for any unhandled types
        //    //        //rendering_logger->PEACH_LOG("Unhandled type in variant for ProcessLoadedResourcePackage", "RenderingManager", LogManager::LogLevel::Warning);
        //    //    }
        //    //    }, ResourcePackage.get()->ResourceData);
        //}
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

    [[nodiscard]] bool 
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

    [[nodiscard]] VulkanRenderer*
        RenderingManager::GetVulkanRenderer()
    {
        return pm_VulkanRenderer.get();
    }
}
