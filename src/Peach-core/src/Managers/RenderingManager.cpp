#include "../../include/Managers/RenderingManager.h"
/*
	This class is used to manage the render thread, and queue/unqueue objects safely
	Copyright(c) 2024-present Ranyodh Singh Mandur.

*/

namespace PeachCore {


    RenderingManager::~RenderingManager() 
    {
        Shutdown();
    }

    RenderingManager::RenderingManager()
    {

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

    bool RenderingManager::CreateWindowAndCamera2D(const string& fp_Title, int fp_Width, int fp_Height)
    {
        if (pm_CurrentWindow || pm_CurrentCamera2D)
        {
            LogManager::RenderingLogger().Warn("RenderingManager already has a valid window or camera2D currently active", "RenderingManager");
            return false;
        }

        // Create an SFML window and context settings
/*       sf::ContextSettings settings;
       settings.depthBits = 24;
       settings.stencilBits = 8; //THESE SETTINGS ARE BROKEN FOR SOME REASON, AND I DONT KNOW WHY
       settings.antialiasingLevel = 4;
       settings.majorVersion = 3;
       settings.minorVersion = 3;
       settings.attributeFlags = sf::ContextSettings::Core;*/

        pm_CurrentWindow = new sf::RenderWindow(sf::VideoMode(fp_Width, fp_Height), fp_Title, sf::Style::Default);

        // Camera Setup
        pm_CurrentCamera2D = new PeachCamera2D(*pm_CurrentWindow);
        pm_CurrentCamera2D->SetCenter(400, 300); // Set this dynamically as needed
        pm_CurrentCamera2D->SetSize(800, 600); // Set this to zoom in or out
        pm_CurrentCamera2D->Enable();

        if (!pm_CurrentWindow->isOpen())
        {
            throw runtime_error("Failed to create window.");
            return false;
        }

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
                visit(overloaded{
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
        while (pm_LoadedResourceQueue->PopLoadedResourceQueue(ResourcePackage)) {
            visit(overloaded{
                [&]( unique_ptr<unsigned char>& fp_RawByteData)
                {
                    // Handle creation logic here
                },
                [&](unique_ptr<sf::Texture>& fp_TextureData)
                {
                    m_TestTexture = move(fp_TextureData);
                },
                [&](unique_ptr<string>& fp_JSONData)
                {
                    // Handle deletion logic here
                    // Ensure resources are properly released and objects are cleaned up
                }
                }, ResourcePackage.get()->ResourceData);
        }
    }

    void 
        RenderingManager::RenderFrame() 
    {
        ProcessLoadedResourcePackages(); //move all loaded objects into memory here if necessary
        //ProcessCommands(); //process all updates

        //sf::Sprite sprite;
        //sprite.setTexture(*m_TestTexture);

        //// Get the size of the window
        //sf::Vector2u windowSize = pm_CurrentWindow->getSize();

        //// Get the size of the texture
        //sf::Vector2u textureSize = m_TestTexture->getSize();

        //// Calculate scale factors
        //float scaleX = float(windowSize.x) / textureSize.x;
        //float scaleY = float(windowSize.y) / textureSize.y;

        //// Set the scale of the sprite
        //sprite.setScale(scaleX, scaleY);

        //sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
        //sprite.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f);

        // Main loop that continues until the window is closed
        while (pm_CurrentWindow->isOpen()) 
        {
            if (pm_IsShutDown) //used to stop rendering loop if possible when ForceQuit() is called
            {
                pm_CurrentWindow->clear();
                pm_IsShutDown = false; //gotta reset it otherwise everytime we run the scene again it just closes immediately lmao
                break;
            }
            // Process events
            sf::Event event;

            while (pm_CurrentWindow->pollEvent(event)) 
            {
                if (event.type == sf::Event::Closed)
                {
                    pm_CurrentWindow->close();
                }
            }

            // Clear the screen with a dark blue color
            pm_CurrentWindow->clear(sf::Color(0, 0, 139));

            // Draw the sprite
            //pm_CurrentWindow->draw(sprite);

            // Update the window
            pm_CurrentWindow->display();
        }
    }

    void RenderingManager::BeginFrame()
    {
        //BeginDrawing();
    }

    void RenderingManager::EndFrame() 
    {
        if (pm_CurrentWindow)
        {
            pm_CurrentWindow->display();
        }
    }

    void RenderingManager::Shutdown()
    {
        if(pm_CurrentCamera2D)
        {
            delete pm_CurrentCamera2D;
            pm_CurrentCamera2D = nullptr;
        }
        if(pm_CurrentWindow)
        {
            delete pm_CurrentWindow;
            pm_CurrentWindow = nullptr;
        }
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