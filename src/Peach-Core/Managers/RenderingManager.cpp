#include "../../include/Peach-Core/Managers/RenderingManager.h"
/*
	This class is used to manage the render thread, and queue/unqueue objects safely
	Copyright(c) 2024-present Ranyodh Singh Mandur.

*/

namespace PeachCore {


    RenderingManager::~RenderingManager() 
    {
        if (pm_PeachRenderer)
        {
            pm_PeachRenderer.reset(nullptr);
        }
    }

    void 
        RenderingManager::Shutdown()
    {
        if (pm_PeachRenderer)
        {
            //SDL_DestroyWindow(pm_MainWindow);

            ////delete pm_MainWindow; //WARNING: DO NOT UNCOMMENT THIS, IT WILL CAUSE A HEAP MEMORY VIOLATION
            //pm_MainWindow = nullptr;
        }
    }

    SDL_Window*
        RenderingManager::CreateSDLWindow
        (
            const char* fp_WindowTitle,
            const unsigned int fp_WindowWidth,
            const unsigned int fp_WindowHeight
        )
        const
    {
        if (not pm_IsInitialized)
        {
            LogManager::RenderingLogger().LogAndPrint("Please initialize RenderingManager before trying to create a window!", "RenderingManager", "warn");
            return nullptr;
        }

        return SDL_CreateWindow
                (
                    fp_WindowTitle,
                    fp_WindowWidth, 
                    fp_WindowHeight,
                    SDL_WINDOW_OPENGL
                );
    }

    bool
        RenderingManager::CreatePeachRenderer(SDL_Window* fp_Window)
    {
        if (not fp_Window)
        {
            LogManager::RenderingLogger().LogAndPrint("Please try creating an SDL window before trying to create a PeachRenderer!", "RenderingManager", "warn");
            return false;
        }

        if (pm_PeachRenderer)
        {
            pm_PeachRenderer.reset(nullptr);
        }

        pm_PeachRenderer = make_unique<PeachRenderer>(fp_Window);
        return true;
    }

    void
        RenderingManager::DestroyPeachRenderer()
    {
        //pm_PeachRenderer->Destroy();
        pm_PeachRenderer.reset(nullptr);
    }

    //creates a window and opengl context, enables sfml 2d graphics and such as well, returns the command queue for thread safe control
    shared_ptr<CommandQueue> 
        RenderingManager::InitializeQueues()
    {
        if (pm_CommandQueue || pm_LoadedResourceQueue)
        {
            LogManager::RenderingLogger().LogAndPrint("RenderingManager already initialized.", "RenderingManager", "warn");
            return nullptr;
        }

        pm_CommandQueue = make_shared<CommandQueue>();
        pm_LoadedResourceQueue = ResourceLoadingManager::ResourceLoader().GetDrawableResourceLoadingQueue();

        LogManager::RenderingLogger().LogAndPrint("RenderingManager successfully initialized >w<", "RenderingManager", "debug");

        pm_AreQueuesInitialized = true;

        return pm_CommandQueue; //returns one and only one ptr to whoever initializes RenderingManager, this is meant only for the main thread
    }

    bool
        RenderingManager::InitializeOpenGL()
    {
        if (pm_IsInitialized)
        {
            LogManager::RenderingLogger().LogAndPrint("PeachEditorRenderingManager tried to initialize OpenGL when rendering has already been initialized", "PeachEditorRenderingManager", "warn");
            return false;
        }

        if (not pm_AreQueuesInitialized)
        {
            LogManager::RenderingLogger().LogAndPrint("PeachEditorRenderingManager tried to initialize OpenGL before initializing command/loading queues!", "PeachEditorRenderingManager", "warn");
            return false;
        }

        if (not pm_PeachRenderer->GetMainWindow())
        {
            LogManager::RenderingLogger().LogAndPrint("PeachEditorRenderingManager tried to initialize OpenGL before creating the main window!", "PeachEditorRenderingManager", "warn");
            return false;
        }

        if (glewInit() != GLEW_OK)
        {
            LogManager::RenderingLogger().LogAndPrint("Failed to create GLEW context: " + static_cast<string>("OWO"), "PeachEditorRenderingManager", "fatal");
            SDL_DestroyWindow(pm_PeachRenderer->GetMainWindow());
            return false;
        }

        LogManager::RenderingLogger().LogAndPrint("GLEW initialized properly", "PeachEditorRenderingManager", "debug");

        rendering_logger = &LogManager::RenderingLogger();

        pm_IsInitialized = true;

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
                    LogManager::RenderingLogger().LogAndPrint("Unhandled type in variant for ProcessLoadedResourcePackage", "PeachEditorRenderingManager", "warn");
                }
                }, ResourcePackage.get()->ResourceData);
        }
    }

    void 
        RenderingManager::RenderFrame(bool fp_IsStressTest)
    {
        if (not pm_IsInitialized)
        {
            LogManager::RenderingLogger().LogAndPrint("Please initialize RenderingManager before trying to render anything!", "RenderingManager", "warn");
            return;
        }

        if (not pm_PeachRenderer->GetMainWindow())
        {
            LogManager::RenderingLogger().LogAndPrint("Please assign a valid SDL window to pm_MainWindow before trying to render!", "RenderingManager", "warn");
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

    PeachRenderer*
        RenderingManager::GetPeachRenderer()
    {
        return pm_PeachRenderer.get();
    }
}