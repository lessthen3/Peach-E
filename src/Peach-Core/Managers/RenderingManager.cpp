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
        if (pm_CurrentCamera2D)
        {
            delete pm_CurrentCamera2D;
            pm_CurrentCamera2D = nullptr;
        }
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

    bool RenderingManager::CreateSDLWindow(const char* fp_WindowTitle, const int fp_WindowWidth, const int fp_WindowHeight)
    {
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

    //creates a window and opengl context, enables sfml 2d graphics and such as well, returns the command queue for thread safe control
    shared_ptr<CommandQueue> 
        RenderingManager::Initialize()
    {
        if (pm_CommandQueue || pm_LoadedResourceQueue)
        {
            LogManager::RenderingLogger().Warn("RenderingManager already initialized.", "RenderingManager");
            return nullptr;
        }

        pm_CommandQueue = make_shared<CommandQueue>();
        pm_LoadedResourceQueue = ResourceLoadingManager::ResourceLoader().GetDrawableResourceLoadingQueue();

        LogManager::RenderingLogger().Debug("RenderingManager successfully initialized >w<", "RenderingManager");

        return pm_CommandQueue; //returns one and only one ptr to whoever initializes RenderingManager, this is meant only for the main thread
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

            SDL_GL_SwapWindow(pm_CurrentWindow);

            if (fp_IsStressTest) //used to stop rendering loop after one cycle for testing
            {
                //pm_CurrentWindow->clear();
                //pm_CurrentWindow->close();
                pm_IsShutDown = false; //gotta reset it otherwise everytime we run the scene again it just closes immediately lmao
                break;
            }
        }

        Shutdown();
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