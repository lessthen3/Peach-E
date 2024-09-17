#include "../../include/Peach-Core/Managers/RenderingManager.h"
/*
	This class is used to manage the render thread, and queue/unqueue objects safely
	Copyright(c) 2024-present Ranyodh Singh Mandur.

*/

namespace PeachCore {


    RenderingManager::~RenderingManager() 
    {
    }

    void 
        RenderingManager::Shutdown()
    {
        if (pm_CurrentWindow)
        {
            SDL_DestroyWindow(pm_CurrentWindow);
            //SDL_Quit();

            //delete pm_CurrentWindow; //WARNING: DO NOT UNCOMMENT THIS, IT WILL CAUSE A HEAP MEMORY VIOLATION
            pm_CurrentWindow = nullptr;
        }
 /*       if (m_TestTexture)
        {
            m_TestTexture.reset(nullptr);
        }*/
    }

    bool 
        RenderingManager::CreateSDLWindow
        (
            const char* fp_WindowTitle, 
            const unsigned int fp_WindowWidth, 
            const unsigned int fp_WindowHeight
        )
    {
        if (not pm_IsInitialized)
        {
            LogManager::RenderingLogger().Warn("Please initialize RenderingManager before trying to create a window!", "RenderingManager");
            return false;
        }

        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            LogManager::RenderingLogger().Fatal("SDL could not initialize! SDL_Error: " + string(SDL_GetError()), "RenderingManager");
            return false;
        }

        pm_CurrentWindow = SDL_CreateWindow(
            fp_WindowTitle,
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            fp_WindowWidth, fp_WindowHeight,
            SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

        if (!pm_CurrentWindow)
        {
            LogManager::RenderingLogger().Fatal("Window could not be created! SDL_Error: " + string(SDL_GetError()), "RenderingManager");
            SDL_Quit();
            return false;
        }

        return true;
    }

    bool
        RenderingManager::CreatePeachRenderer()
    {
        if (pm_PeachRenderer)
        {
            delete pm_PeachRenderer;
            pm_PeachRenderer = nullptr;
        }

        if(not pm_CurrentWindow)
        {
            LogManager::RenderingLogger().Warn("Please try creating an SDL window before trying to create a PeachRenderer!", "RenderingManager");
            return false;
        }

        pm_PeachRenderer = new PeachRenderer(pm_CurrentWindow);
        return true;
    }

    //creates a window and opengl context, enables sfml 2d graphics and such as well, returns the command queue for thread safe control
    shared_ptr<CommandQueue> 
        RenderingManager::InitializeQueues()
    {
        if (pm_CommandQueue || pm_LoadedResourceQueue)
        {
            LogManager::RenderingLogger().Warn("RenderingManager already initialized.", "RenderingManager");
            return nullptr;
        }

        pm_CommandQueue = make_shared<CommandQueue>();
        pm_LoadedResourceQueue = ResourceLoadingManager::ResourceLoader().GetDrawableResourceLoadingQueue();

        LogManager::RenderingLogger().Debug("RenderingManager successfully initialized >w<", "RenderingManager");

        pm_IsInitialized = true;

        return pm_CommandQueue; //returns one and only one ptr to whoever initializes RenderingManager, this is meant only for the main thread
    }

    bool 
        RenderingManager::InitializeOpenGL
        (
        )
    {
        if (not pm_CurrentWindow)
        {
            LogManager::RenderingLogger().Warn("RenderingManager tried to initialize opengl before creating the main window!", "RenderingManager");
            return false;
        }

        int f_WindowWidth, f_WindowHeight;
        SDL_GetWindowSize(pm_CurrentWindow, &f_WindowWidth, &f_WindowHeight);

        SDL_SysWMinfo wmi;
        SDL_VERSION(&wmi.version);

        if (not SDL_GetWindowWMInfo(pm_CurrentWindow, &wmi)) 
        {
            cerr << "Unable to get window info: " << SDL_GetError() << endl;
            throw runtime_error("Failed to get window manager info.");
        }
        else
        {
            LogManager::RenderingLogger().Debug("SDL window info successfully read", "PeachRenderer");
        }

        bgfx::Init bgfxInit;

        #if defined(_WIN32) || (_WIN64)
                    bgfxInit.platformData.nwh = wmi.info.win.window;  // Windows
        #elif defined(__linux__)
                    bgfxInit.platformData.nwh = (void*)wmi.info.x11.window;  // Linux
        #elif defined(__APPLE__)
                    bgfxInit.platformData.nwh = wmi.info.cocoa.window;  // macOS
        #endif

        bgfxInit.platformData.ndt = NULL;
        bgfxInit.platformData.context = NULL;
        bgfxInit.platformData.backBuffer = NULL;
        bgfxInit.platformData.backBufferDS = NULL;

        bgfxInit.type = bgfx::RendererType::OpenGL; // Use the specified type or auto-select

        bgfxInit.resolution.width = f_WindowWidth;
        bgfxInit.resolution.height = f_WindowHeight;
        bgfxInit.resolution.reset = BGFX_RESET_VSYNC;

        if (not bgfx::init(bgfxInit))
        {
            LogManager::RenderingLogger().Fatal("BGFX failed to initialize. RIP", "PeachRenderer");
            throw runtime_error("BGFX initialization failed");
        }
        else
        {
            LogManager::RenderingLogger().Debug("BGFX initialized properly", "PeachRenderer");
        }

        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
        bgfx::setViewRect(0, 0, 0, f_WindowWidth, f_WindowHeight);

        bgfx::frame();
            
        return true;
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
                    // Default handler for any unhandled types
                    LogManager::RenderingLogger().Warn("Unhandled type in variant for ProcessLoadedResourcePackage", "PeachEditorRenderingManager");
                }
                }, ResourcePackage.get()->ResourceData);
        }
    }

    void 
        RenderingManager::RenderFrame(bool fp_IsStressTest)
    {
        if (not pm_IsInitialized)
        {
            LogManager::RenderingLogger().Warn("Please initialize RenderingManager before trying to render anything!", "RenderingManager");
            return;
        }

        if (not pm_CurrentWindow)
        {
            LogManager::RenderingLogger().Warn("Please assign a valid SDL window to pm_CurrentWindow before trying to render!", "RenderingManager");
            return;
        }

        //ProcessLoadedResourcePackages(); //move all loaded objects into memory here if necessary
        //ProcessCommands(); //process all updates

        bool f_IsGameRuntimeOver = false;

        // Main loop that continues until the window is closed
        while (!f_IsGameRuntimeOver)
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

                if (event.type == SDL_QUIT)
                {
                    f_IsGameRuntimeOver = true;
                }
            }

            bgfx::frame(); //advance the frame

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

    unsigned int 
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
}