#include "../../include/Managers/PeachEngineRenderingManager.h"
/*
    This class is used to manage the render thread, and queue/unqueue objects safely
    Copyright(c) 2024-present Ranyodh Singh Mandur.

*/

namespace PeachEditor {


    PeachEngineRenderingManager::~PeachEngineRenderingManager()
    {
        delete pm_Camera2D;
        delete pm_CurrentWindow;
    }

    PeachEngineRenderingManager::PeachEngineRenderingManager()
    {

    }

    //creates a window and opengl context, enables sfml 2d graphics and such as well, returns the command queue for thread safe control
    shared_ptr<CommandQueue>
        PeachEngineRenderingManager::Initialize(const string& fp_Title, int fp_Width, int fp_Height)
    {
        if (pm_HasBeenInitialized)
        {
            InternalLogManager::InternalRenderingLogger().Warn("RenderingManager already initialized.", "RenderingManager");
            return nullptr;
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
        pm_Camera2D = new PeachCamera2D(*pm_CurrentWindow);
        pm_Camera2D->SetCenter(400, 300); // Set this dynamically as needed
        pm_Camera2D->SetSize(800, 600); // Set this to zoom in or out
        pm_Camera2D->Enable();

        if (!pm_CurrentWindow->isOpen())
        {
            throw runtime_error("Failed to create window.");
        }

        pm_CommandQueue = make_shared<CommandQueue>();
        pm_LoadedResourceQueue = ResourceLoadingManager::ResourceLoader().GetDrawableResourceLoadingQueue();

        InternalLogManager::InternalRenderingLogger().Debug("RenderingManager successfully initialized >w<", "RenderingManager");

        pm_HasBeenInitialized = true;

        CreatePeachEConsole();
        SetupRenderTexture(pm_CurrentWindow->getSize().x, pm_CurrentWindow->getSize().y);

        return pm_CommandQueue; //returns one and only one ptr to whoever initializes RenderingManager, this is meant only for the main thread
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

    void PeachEngineRenderingManager::RenderFrame()
    {
        ProcessLoadedResourcePackages(); //move all loaded objects into memory here if necessary
        //ProcessCommands(); //process all updates
                // Main loop that continues until the window is closed

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

        bool f_ShouldConsoleBeOpen = true;

        ImGui::SFML::Init(*pm_CurrentWindow); 	// sets up ImGui with ether a dark or light default theme

        ImGuiIO& f_ImGuiIO = ImGui::GetIO();
        f_ImGuiIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking

        sf::Clock f_DeltaClock;

        PeachCore::RenderingManager::Renderer().Initialize();

        //////////////////////////////////////////////
        // MAIN RENDER LOOP FOR THE EDITOR
        //////////////////////////////////////////////

        while (pm_CurrentWindow->isOpen())
        {
            Clear(); //clears screen and beings drawing

            //////////////////////////////////////////////
            // Input Polling for Imgui/SFML
            //////////////////////////////////////////////

            sf::Event event;

            while (pm_CurrentWindow->pollEvent(event))
            {
                ImGui::SFML::ProcessEvent(event);

                if (event.type == sf::Event::Closed) 
                {
                    pm_CurrentWindow->close();
                }

                // Handle window resize
                if (event.type == sf::Event::Resized) 
                {
                    // Recreate the render texture with new dimensions
                    ResizeRenderTexture(event.size.width, event.size.height);
                    pm_Camera2D->SetSize(event.size.width, event.size.height); // Adjust camera as well if necessary
                    pm_Camera2D->SetCenter(event.size.width * 0.50f, event.size.height * 0.50f);
                    pm_CurrentWindow->setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
                }
            }

            //////////////////////////////////////////////
            // Update Imgui UI
            //////////////////////////////////////////////

            ImGui::SFML::Update(*pm_CurrentWindow, f_DeltaClock.restart()); // starts the ImGui content mode. Make all ImGui calls after this

            //////////////////////////////////////////////
            // Render Texture Setup
            //////////////////////////////////////////////

            if (pm_ViewportRenderTexture)
            {
                pm_ViewportRenderTexture->clear(sf::Color(128, 128, 128)); // Clear with grey >w<, or any color you need

                pm_ViewportRenderTexture->display(); // Updates the texture with what has been drawn

                // Initialize Dockspace
                ImGuiID dockspace_id = ImGui::GetID("ViewportDockspace");
                ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

                ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                                                       ImGuiWindowFlags_NoResize    | ImGuiWindowFlags_NoMove |
                                                                       ImGuiWindowFlags_NoScrollbar| ImGuiWindowFlags_NoScrollWithMouse;

                //ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.5f); // Adjust border size
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));  // Set padding to zero for current window
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // Set border color to white


                ImGui::SetNextWindowPos(ImVec2(0, 0));
                ImGui::SetNextWindowSize(ImVec2(pm_CurrentWindow->getSize().x * 0.70f, pm_CurrentWindow->getSize().y *0.60f)); //SIZE OF IMGUI VIEWPORT
                //ImGui::SetNextWindowDockID(ImGui::GetID("MyDockSpace"), ImGuiCond_FirstUseEver);

                if (ImGui::Begin("Viewport", nullptr, window_flags))
                {
                   // ImVec2 content_region_avail = ImGui::GetContentRegionAvail(); // Get available space in the window
                    // Display texture fitted to the available window space
                    ImGui::Image(*pm_ViewportRenderTexture);
                }

                ImGui::End();
                ImGui::PopStyleVar(); // Pop border size
                ImGui::PopStyleColor(); // Pop border color
            }

            //////////////////////////////////////////////
            // Virtual File System Setup
            //////////////////////////////////////////////

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
                        Clear(); // IMPORTANT: NEEDA EXPLICITLY ENDDRAWING() BEFORE CALLING CLOSEWINDOW!
                        m_IsSceneCurrentlyRunning = false;
                        PeachCore::RenderingManager::Renderer().ForceQuit();
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
                    if (ImGui::MenuItem("Run Peach-E Project") && !m_IsSceneCurrentlyRunning)
                    {
                        // Run the game in a new window
                        thread T_CurrentSceneRunnerThread([]()
                            {
                                PeachCore::RenderingManager::Renderer().CreateWindowAndCamera2D("Peach Game", 800, 600);
                                PeachCore::RenderingManager::Renderer().RenderFrame();
                                PeachCore::RenderingManager::Renderer().Shutdown();
                                
                                PeachEngineRenderingManager::PeachEngineRenderer().m_IsSceneCurrentlyRunning = false;
                            });
                        T_CurrentSceneRunnerThread.detach();
                        m_IsSceneCurrentlyRunning = true;
                    }
                    if (ImGui::MenuItem("Force Quit Peach-E Project"))
                    {
                        m_IsSceneCurrentlyRunning = false;
                        PeachCore::RenderingManager::Renderer().ForceQuit();
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

            ImVec2 viewportSize = ImGui::GetContentRegionAvail();
            ImVec2 viewportPos = ImGui::GetCursorScreenPos();

            // Get mouse position in viewport
            ImVec2 mousePos = ImGui::GetMousePos();
            bool isMouseInViewport = ImGui::IsMouseHoveringRect(viewportPos, ImVec2(viewportPos.x + viewportSize.x, viewportPos.y + viewportSize.y));

            if (isMouseInViewport)
            {
                //// Convert ImGui mouse coordinates to viewport coordinates
                //Vector2 mousePosInViewport = { mousePos.x - viewportPos.x, mousePos.y - viewportPos.y };

                //// Handle drag and drop or selection
                //if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                //    // Handle mouse click
                //}
                //if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                //    // Handle mouse drag
                //}
            }


            //////////////////////////////////////////////
            // Scene Tree View Panel
            //////////////////////////////////////////////

            // Panels
            ImGui::Begin("Hierarchy");
            ImGui::Text("Scene Hierarchy");
            ImGui::End();

            //////////////////////////////////////////////
            // Peach-E Console
            //////////////////////////////////////////////

            //ImGui::Begin("Console DockSpace");

            //ImGuiID console_dock_space_ID = ImGui::GetID("Console DockSpace");
            //ImGui::DockSpace(console_dock_space_ID);

            //ImGui::SetNextWindowDockID(console_dock_space_ID, ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowPos(ImVec2(0, pm_CurrentWindow->getSize().y * 0.60f)); //start at the y point thats right "above" (below in this context) the viewport
            ImGui::SetNextWindowSize(ImVec2(pm_CurrentWindow->getSize().x * 0.70f, pm_CurrentWindow->getSize().y * 0.40f)); //SIZE OF PEACH-E CONSOLE

            pm_EditorConsole->Draw("PEACH CONSOLE", &f_ShouldConsoleBeOpen);

            //ImGui::End(); //End Console DockSpace

            ImGui::SFML::Render(*pm_CurrentWindow);			// ends the ImGui content mode. Make all ImGui calls before this

            // Draw the sprite
            //pm_CurrentWindow->draw(sprite);

            // Update the window
            pm_CurrentWindow->display();
        }

        ImGui::SFML::Shutdown();		// cleans up ImGui
    }

    // Call this method to setup the render texture
    bool PeachEngineRenderingManager::SetupRenderTexture(const unsigned int fp_Width, const unsigned int fp_Height, bool IsNearestNeighbour) //pass in screen width and size, and it scales it to the desired proportions automatically
    {
        if (pm_ViewportRenderTexture)
        {
            InternalLogManager::InternalRenderingLogger().Warn("Attempted to setup render texture again when a valid instance of pm_ViewportRenderTexture is running", "PeachEngineRenderingManager");
            return false;
        }

        pm_ViewportRenderTexture = make_unique<sf::RenderTexture>();

        if (pm_ViewportRenderTexture->create(fp_Width * 0.70f, fp_Height * 0.60f) && !IsNearestNeighbour)
        {
            pm_ViewportRenderTexture->setSmooth(true);
        }
        else if (pm_ViewportRenderTexture->create(fp_Width * 0.70f, fp_Height * 0.60f) && IsNearestNeighbour)
        {
            pm_ViewportRenderTexture->setSmooth(false);
        }
        else
        {
            InternalLogManager::InternalRenderingLogger().Warn("Failed to create a valid instance of pm_ViewportRenderTexture, setting it back to nullptr", "PeachEngineRenderingManager");
            pm_ViewportRenderTexture = nullptr;
            return false;
        }

        return true;
    }

    bool PeachEngineRenderingManager::ResizeRenderTexture(const unsigned int fp_Width, const unsigned int fp_Height, bool IsNearestNeighbour)  //pass in screen width and size, and it scales it to the desired proportions automatically
    {
        if (pm_ViewportRenderTexture->create(fp_Width * 0.70f, fp_Height * 0.60f) && !IsNearestNeighbour)
        {
            pm_ViewportRenderTexture->setSmooth(true);
        }
        else if (pm_ViewportRenderTexture->create(fp_Width * 0.70f, fp_Height * 0.60f) && IsNearestNeighbour)
        {
            pm_ViewportRenderTexture->setSmooth(false);
        }
        else
        {
            InternalLogManager::InternalRenderingLogger().Warn("Failed to resize a valid instance of pm_ViewportRenderTexture, setting it back to nullptr", "PeachEngineRenderingManager");
            pm_ViewportRenderTexture = nullptr;
            return false;
        }

        return true;
    }

    void PeachEngineRenderingManager::RunCurrentScene()
    {

    }

    void PeachEngineRenderingManager::Clear()
    {
        if (pm_CurrentWindow)
        {
            pm_CurrentWindow->clear();
        }
    }

    void PeachEngineRenderingManager::EndFrame()
    {
        if (pm_CurrentWindow)
        {
            pm_CurrentWindow->display();
        }
    }

    void PeachEngineRenderingManager::CreatePeachEConsole()
    {
        pm_EditorConsole = make_unique<PeachEConsole>();
    }

    void PeachEngineRenderingManager::CreateSceneTreeViewPanel()
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