#include "../../include/Managers/PeachEngineRenderingManager.h"
/*
    This class is used to manage the render thread, and queue/unqueue objects safely
    Copyright(c) 2024-present Ranyodh Singh Mandur.

*/

namespace PeachEngine {


    PeachEngineRenderingManager::~PeachEngineRenderingManager()
    {
        //THIS BRICKS EVERYTHING AND IDK Y, IT DELETES GLFW OR SMTH IDFK
        //EndDrawing(); //IMPORTANT: need to end drawing before closing window, or else we will have an ungraceful exit from the program
        //rlImGuiShutdown();		// cleans up ImGui
        //CloseWindow(); // Close the window
    }

    PeachEngineRenderingManager::PeachEngineRenderingManager()
    {

    }

    //creates a window and opengl context, enables sfml 2d graphics and such as well, returns the command queue for thread safe control
    shared_ptr<PeachCore::CommandQueue> 
        PeachEngineRenderingManager::Initialize(const string& fp_Title, int fp_Width, int fp_Height)
    {
        if (pm_HasBeenInitialized)
        {
            InternalLogManager::InternalRenderingLogger().Warn("PeachEngineRenderingManager already initialized.", "PeachEngineRenderingManager");
            return nullptr;
        }

        InitWindow(fp_Width, fp_Height, fp_Title.c_str());

        if (!IsWindowReady())
        {
            throw runtime_error("Failed to create window.");
        }

        // Camera Setup, stack allocated for now
        pm_Camera2D = PeachCore::PeachCamera2D(fp_Width, fp_Height);
        pm_Camera2D.SetCenter(400, 300); // Set this dynamically as needed
        pm_Camera2D.SetSize(800, 600); // Set this to zoom in or out
        pm_Camera2D.Apply();

        pm_CommandQueue = make_shared<PeachCore::CommandQueue>();
        pm_LoadedResourceQueue = PeachEngineResourceLoadingManager::PeachEngineResourceLoader().GetDrawableResourceLoadingQueue();

        InternalLogManager::InternalRenderingLogger().Debug("PeachEngineRenderingManager successfully initialized >w<", "PeachEngineRenderingManager");

        pm_HasBeenInitialized = true;

        return pm_CommandQueue; //returns one and only one ptr to whoever initializes PeachEngineRenderingManager, this is meant only for the main thread
    }

    void
        PeachEngineRenderingManager::ProcessCommands()
    {
        PeachCore::DrawCommand f_DrawCommand;
        while (pm_CommandQueue->PopSendersQueue(f_DrawCommand))
        {
            for (auto& drawable_data : f_DrawCommand.DrawableData)
            {
                visit(
                    PeachCore::overloaded
                    {
                    [&](const vector<PeachCore::CreateDrawableData>& fp_Data)
                    {
                        // Handle creation logic here
                    },
                    [&](const vector<PeachCore::UpdateActiveDrawableData>& fp_Data)
                    {
                        // Handle update logic here
                        // This could involve updating position based on deltaPosition
                        // Setting visibility, layer sorting, etc.
                    },
                    [&](const vector<PeachCore::DeleteDrawableData>& fp_Data)
                    {
                        // Handle deletion logic here
                        // Ensure resources are properly released and objects are cleaned up
                    }
                    }, drawable_data);
            }
        }
    }

    void
        PeachEngineRenderingManager::ProcessLoadedResourcePackages()
    {
        unique_ptr<PeachCore::LoadedResourcePackage> ResourcePackage;
        while (pm_LoadedResourceQueue->PopLoadedResourceQueue(ResourcePackage))
        {
            visit(
                PeachCore::overloaded
                {
                [&](unique_ptr<unsigned char>& fp_RawByteData)
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

    void PeachEngineRenderingManager::RenderFrame()
    {
        ProcessLoadedResourcePackages(); //move all loaded objects into memory here if necessary
        //ProcessCommands(); //process all updates
                // Main loop that continues until the window is closed

        float sliderValue = 50.0f;
        char textBoxInput[64] = "Type here";

        rlImGuiSetup(true); 	// sets up ImGui with ether a dark or light default theme

        while (!WindowShouldClose())
        {
            Clear(); //clears screen and beings drawing
            //pm_Camera2D.Apply(); //already applide in Initialize() method

            // Draw background texture if a custom background is set
            if (true) 
            {
                Texture2D f_DereferencedTexture2D = *m_TestTexture.get();

                DrawTexturePro
                (
                    f_DereferencedTexture2D, //dereference smart ptr

                    { 0, 0, (float)f_DereferencedTexture2D.width, (float)f_DereferencedTexture2D.height },

                    { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f, (float)f_DereferencedTexture2D.width, (float)f_DereferencedTexture2D.height },

                    { (float)f_DereferencedTexture2D.width / 2.0f, (float)f_DereferencedTexture2D.height / 2.0f },

                    0.0f,

                    WHITE
                );
            }

            // inside your game loop, between BeginDrawing() and EndDrawing()
            rlImGuiBegin();			// starts the ImGui content mode. Make all ImGui calls after this


            // Your ImGui code goes here
            ImGui::Begin("Editor");
            ImGui::Text("Hello, Raylib ImGui!");
            ImGui::End();

            //////////////////////////////////////////////
            // Menu Bar Setup
            //////////////////////////////////////////////

            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("New"))
                    {
                        // New action
                    }
                    if (ImGui::MenuItem("Open"))
                    {
                        // Open action
                    }
                    if (ImGui::MenuItem("Save"))
                    {
                        // Save action
                    }
                    if (ImGui::MenuItem("Exit")) //THIS CRASHES EVERYTHING BUT I GUESS THAT IS A WAY TO CLOSE STUFF
                    {
                        EndDrawing(); // IMPORTANT: NEEDA EXPLICITLY ENDDRAWING() BEFORE CALLING CLOSEWINDOW!
                        break;
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Edit"))
                {
                    if (ImGui::MenuItem("Delete"))
                    {
                        // New action
                    }
                    if (ImGui::MenuItem("Duplicate"))
                    {
                        // Open action
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Run"))
                {
                    if (ImGui::MenuItem("Run Peach-E Project"))
                    {
                        // Run the game in a new window
                        thread gameThread([]() 
                            {
                            PeachCore::RenderingManager::Renderer().Initialize("Peach Engine Game", 800, 600);
                            
                            //PeachCore::RenderingManager::Renderer().RenderFrame();
                            
                            //PeachCore::RenderingManager::Renderer().Shutdown();
                            });
                        gameThread.detach();
                    }
                    if (ImGui::MenuItem("Build Peach-E Project"))
                    {
                        // Open action
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Project"))
                {
                    if (ImGui::MenuItem("Settings"))
                    {
                        // New action
                    }
                    if (ImGui::MenuItem("Input Map"))
                    {
                        // Open action
                    }
                    if (ImGui::MenuItem("Refresh Project"))
                    {
                        // Open action
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("View"))
                {
                    if (ImGui::MenuItem("Delete"))
                    {
                        // New action
                    }
                    if (ImGui::MenuItem("Duplicate"))
                    {
                        // Open action
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Tools"))
                {
                    if (ImGui::MenuItem("Open Terminal"))
                    {
                        // New action
                    }
                    if (ImGui::MenuItem("Editor Theme"))
                    {
                        // Open action
                    }
                    if (ImGui::MenuItem("Editor Settings"))
                    {
                        // Open action
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Plugins"))
                {
                    if (ImGui::MenuItem("Load Plugin"))
                    {
                        // New action
                    }
                    if (ImGui::MenuItem("List of Active Plugins"))
                    {
                        // Open action
                    }
                    ImGui::EndMenu();
                }

                ImGui::EndMainMenuBar();
            }

            //////////////////////////////////////////////
            // Setting Up Scene View
            //////////////////////////////////////////////

            //ImGui::Begin("Viewport");

            ImVec2 viewportSize = ImGui::GetContentRegionAvail();
            ImVec2 viewportPos = ImGui::GetCursorScreenPos();

            // Get mouse position in viewport
            ImVec2 mousePos = ImGui::GetMousePos();
            bool isMouseInViewport = ImGui::IsMouseHoveringRect(viewportPos, ImVec2(viewportPos.x + viewportSize.x, viewportPos.y + viewportSize.y));

            if (isMouseInViewport)
            {
                // Convert ImGui mouse coordinates to viewport coordinates
                Vector2 mousePosInViewport = { mousePos.x - viewportPos.x, mousePos.y - viewportPos.y };

                // Handle drag and drop or selection
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    // Handle mouse click
                }
                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    // Handle mouse drag
                }
            }



            // Panels
            ImGui::Begin("Hierarchy");
            ImGui::Text("Scene Hierarchy");
            ImGui::End();

            ImGui::Begin("Inspector");
            ImGui::Text("Inspector");
            ImGui::End();




            rlImGuiEnd();			// ends the ImGui content mode. Make all ImGui calls before this

            EndDrawing(); // Update the window
        }

        //TO-DO: set flags that indicate the current state of closewindow and imguishutdown, because if we call them again in the destructor it realllllllly doesn't like that
        // after your game loop is over, before you close the window
        rlImGuiShutdown();		// cleans up ImGui

        CloseWindow(); // Close the window
    }

    // Call this method to setup the render texture
    //void PeachEngineRenderingManager::SetupRenderTexture(unsigned int width, unsigned int height)
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
    //const sf::Texture& PeachEngineRenderingManager::GetRenderTexture() 
    //    const 
    //{
    //    return renderTexture.getTexture();
    //}

    void PeachEngineRenderingManager::Clear()
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);  // Sets background color
    }

    void PeachEngineRenderingManager::BeginFrame()
    {
        BeginDrawing();
    }

    void PeachEngineRenderingManager::EndFrame()
    {
        EndDrawing();
    }

    void PeachEngineRenderingManager::ResizeWindow()
    {

    }

    string PeachEngineRenderingManager::GetRendererType() const
    {
        return pm_RendererType;
    }
    void PeachEngineRenderingManager::GetCurrentViewPort()
    {

    }
    unsigned int PeachEngineRenderingManager::GetFrameRateLimit() const
    {
        return pm_FrameRateLimit;
    }
    bool PeachEngineRenderingManager::IsVSyncEnabled() const
    {
        return pm_IsVSyncEnabled;
    }
    void PeachEngineRenderingManager::SetVSync(const bool fp_IsEnabled)
    {
        if (fp_IsEnabled)
        {
            pm_IsVSyncEnabled = fp_IsEnabled;
        }
    }
    void PeachEngineRenderingManager::SetFrameRateLimit(unsigned int fp_Limit)
    {
        pm_FrameRateLimit = fp_Limit;
    }
}