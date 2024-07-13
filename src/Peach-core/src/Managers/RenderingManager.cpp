#include "../../include/Managers/RenderingManager.h"
/*
	This class is used to manage the render thread, and queue/unqueue objects safely
	Copyright(c) 2024-present Ranyodh Singh Mandur.

*/

namespace PeachCore {


    RenderingManager::~RenderingManager() 
    {
        if (pm_OpenGLRenderer)
            {delete pm_OpenGLRenderer;}

        if (pm_CurrentWindow)
        {
            SDL_DestroyWindow(pm_CurrentWindow);
            SDL_Quit();
        }
    } 

    void RenderingManager::Initialize(const std::string fp_RendererType, const std::string& fp_Title, const int fp_Width, const int fp_Height)
    {
        if (hasBeenInitialized)
        {
            LogManager::MainLogger().Warn("RenderingManager has already been initialized, RenderingManager is only allowed to initialize once per run", "RenderingManager");
            return;
        }
        else
        {
            hasBeenInitialized = true;
        }

        

        pm_FrameRateLimit = (60);
        pm_IsVSyncEnabled = (false);

        
    }

    //void RenderingManager::SetRendererType(bgfx::RendererType::Enum type) {
    //    pm_PreferredType = type;
    //}

    void RenderingManager::CreateSDLWindow(const char* title, int width, int height) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            throw std::runtime_error("Failed to initialize SDL.");
        }

        pm_CurrentWindow = SDL_CreateWindow(title,
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            width, height,
            SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
        if (!pm_CurrentWindow) {
            std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            SDL_Quit();
            throw std::runtime_error("Failed to create window.");
        }
    }

    void RenderingManager::CreateRenderer2D() {
        if (!pm_CurrentWindow) {
            throw std::runtime_error("Window not created before building renderer.");
        }
        pm_OpenGLRenderer = new OpenGLRenderer(pm_CurrentWindow, false);
    }

    void RenderingManager::RenderFrame() {
        if (pm_OpenGLRenderer) {
            pm_OpenGLRenderer->RenderFrame();
        }
    }

    void RenderingManager::Clear()
    {
        
    }

    void RenderingManager::ResizeWindow()
    {

    }

    void RenderingManager::BeginFrame()
    {

    }


    void RenderingManager::EndFrame()
    {
       
    }

    std::string RenderingManager::GetRendererType() const
    {
        return pm_RendererType;
    }
    void RenderingManager::GetCurrentViewPort()
    {
      
    }
    unsigned int RenderingManager::GetFrameRateLimit() const
    {
        return pm_FrameRateLimit;
    }
    bool RenderingManager::IsVSyncEnabled() const
    {
        return pm_IsVSyncEnabled;
    }
    void RenderingManager::SetVSync(const bool fp_IsEnabled)
    {
        if (fp_IsEnabled)
        {
            pm_IsVSyncEnabled = fp_IsEnabled;
        }
    }
    void RenderingManager::SetFrameRateLimit(unsigned int fp_Limit)
    {
        pm_FrameRateLimit = fp_Limit;
    }

}