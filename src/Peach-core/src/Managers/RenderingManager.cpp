#include "../../include/Managers/RenderingManager.h"
/*
	This class is used to manage the render thread, and queue/unqueue objects safely
	Copyright(c) 2024-present Ranyodh Singh Mandur.

*/

namespace PeachCore {


    RenderingManager::~RenderingManager() 
    {
        //delete pm_OpenGLRenderer;  // SFML handles window cleanup.
    }

    RenderingManager::RenderingManager()
    {

    }

    //creates a window and opengl context, enables sfml 2d graphics and such as well, returns the command queue for thread safe control
    shared_ptr<CommandQueue> RenderingManager::Initialize(const string& fp_Title, int fp_Width, int fp_Height) 
    {
        if (pm_HasBeenInitialized)
        {
            LogManager::RenderingLogger().Warn("RenderingManager already initialized.", "RenderingManager");
            return nullptr;
        }

        InitWindow(fp_Width, fp_Height, fp_Title.c_str());

        if (!IsWindowReady())
        {
            throw runtime_error("Failed to create window.");
        }

        // Camera Setup, stack allocated for now
        pm_Camera2D =  PeachCamera2D(fp_Width, fp_Height);
        pm_Camera2D.SetCenter(400, 300); // Set this dynamically as needed
        pm_Camera2D.SetSize(800, 600); // Set this to zoom in or out
        pm_Camera2D.Apply();

        pm_CommandQueue = make_shared<CommandQueue>();
        pm_LoadedResourceQueue = ResourceLoadingManager::ResourceLoader().GetDrawableResourceLoadingQueue();

        LogManager::RenderingLogger().Debug("RenderingManager successfully initialized >w<", "RenderingManager");

        pm_HasBeenInitialized = true;

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
                [&](unique_ptr<Texture2D>& fp_TextureData)
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
                // Main loop that continues until the window is closed

        float sliderValue = 50.0f;
        char textBoxInput[64] = "Type here";

        while (!WindowShouldClose()) 
        {
            // Begin drawing
            BeginDrawing();
            ClearBackground(Color({ 0, 0, 139, 255 })); // Clear the screen with a dark blue color

            // Apply camera settings
            pm_Camera2D.Apply();

            // Draw sprite if texture is loaded
            if (true) 
            {
                Texture2D f_DereferencedTexture2D = *m_TestTexture.get();

                DrawTexturePro
                (
                    f_DereferencedTexture2D, //dereferenced smart ptr

                    { 0, 0, (float)f_DereferencedTexture2D.width, (float)f_DereferencedTexture2D.height },

                    { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f, (float)f_DereferencedTexture2D.width, (float)f_DereferencedTexture2D.height },

                    { (float)f_DereferencedTexture2D.width / 2.0f, (float)f_DereferencedTexture2D.height / 2.0f },

                    0.0f,

                    WHITE
                );
            }

            EndDrawing(); // Update the window
        }

        CloseWindow(); // Close the window
    }

    // Call this method to setup the render texture
    //void RenderingManager::SetupRenderTexture(unsigned int width, unsigned int height)
    //{
    //    if (renderTexture.create(width, height)) 
    //    {
    //        textureReady = true;
    //        renderTexture.setSmooth(true);
    //    }

    //    else
    //    {
    //        // Handle error
    //    }
    //}

    // Use this method to get the texture for ImGui display
    //const sf::Texture& RenderingManager::GetRenderTexture() 
    //    const 
    //{
    //    return renderTexture.getTexture();
    //}

    void RenderingManager::Clear() 
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);  // Sets background color
    }

    void RenderingManager::BeginFrame()
    {
        BeginDrawing();
    }

    void RenderingManager::EndFrame() 
    {
        EndDrawing();
    }

    void RenderingManager::ResizeWindow()
    {

    }

    string RenderingManager::GetRendererType() const
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