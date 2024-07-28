#include "../../include/Managers/PeachEditorRenderingManager.h"
/*
    This class is used to manage the render thread, and queue/unqueue objects safely
    Copyright(c) 2024-present Ranyodh Singh Mandur.

*/

namespace PeachEditor {

    static void 
        RunGameInstance()
    {
        // Assuming this function sets up, runs, and tears down the game environment
        PeachCore::RenderingManager::Renderer().CreateWindowAndCamera2D("Peach Game", 800, 600);
        PeachCore::RenderingManager::Renderer().RenderFrame(true); //set IsStressTest to true, renders only one frame then exits immediately
        PeachCore::RenderingManager::Renderer().Shutdown();
    }

    static void 
        StessTest(int numIterations, int delayMs)
    {
        for (int i = 0; i < numIterations; ++i)
        {
            cout << "Iteration: " << i + 1 << endl;

            // Start the game instance in a thread
            thread gameThread(RunGameInstance);

            // Allow the thread some time to start and initialize resources
            this_thread::sleep_for(chrono::milliseconds(delayMs));

            // Assuming the game can be stopped by calling a stop function or similar
            // If your architecture uses a global or static flag to signal shutdown, set it here
            PeachCore::RenderingManager::Renderer().ForceQuit();

            // Wait for the thread to finish execution
            gameThread.join();

            // Allow some time for resources to be fully released
            this_thread::sleep_for(chrono::milliseconds(delayMs));
        }
    }


    PeachEditorRenderingManager::~PeachEditorRenderingManager()
    {
        Shutdown();
    }

    void 
        PeachEditorRenderingManager::Shutdown()
    {
        if (pm_Camera2D)
        {
            delete pm_Camera2D;
            pm_Camera2D = nullptr;
        }
        if (pm_CurrentWindow)
        {
            SDL_DestroyWindow(pm_CurrentWindow);
            SDL_Quit();

           //WARNING: DELETING THE CURRENT WINDOW AFTER DESTROYING THE SDL WINDOW AND CALLING SDL_QUIT() CAUSES A HEAP MEMORY VIOLATION
           // delete pm_CurrentWindow; 
            pm_CurrentWindow = nullptr;
        }
        if (pm_OpenGLRenderer)
        {
            delete pm_OpenGLRenderer;
            pm_OpenGLRenderer = nullptr;
        }
    }

    PeachEditorRenderingManager::PeachEditorRenderingManager()
    {

    }

    bool PeachEditorRenderingManager::CreateSDLWindow(const char* fp_WindowTitle, const int fp_WindowWidth, const int fp_WindowHeight)
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) 
        {
            InternalLogManager::InternalRenderingLogger().Fatal("SDL could not initialize! SDL_Error: " + string(SDL_GetError()), "PeachEditorRenderingManager");
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
            InternalLogManager::InternalRenderingLogger().Fatal("Window could not be created! SDL_Error: " + string(SDL_GetError()), "PeachEditorRenderingManager");
            SDL_Quit();
            return false;
        }

        return true;
    }

    //creates a window and opengl context, enables sfml 2d graphics and such as well, returns the command queue for thread safe control
    shared_ptr<PeachCore::CommandQueue>
        PeachEditorRenderingManager::Initialize(const string& fp_Title, int fp_Width, int fp_Height)
    {
        if (pm_HasBeenInitialized)
        {
            InternalLogManager::InternalRenderingLogger().Warn("RenderingManager already initialized.", "RenderingManager");
            return nullptr;
        }

        if (!CreateSDLWindow(fp_Title.c_str(), fp_Width, fp_Height))
        {
            throw runtime_error("Couldn't even start up a window for the editor oof");
        }

        // Camera Setup
        //pm_Camera2D = new PeachCore::PeachCamera2D(*pm_CurrentWindow);
        //pm_Camera2D->SetCenter(400, 300); // Set this dynamically as needed
        //pm_Camera2D->SetSize(800, 600); // Set this to zoom in or out
        //pm_Camera2D->Enable();


        pm_CommandQueue = make_shared<PeachCore::CommandQueue>();
        pm_LoadedResourceQueue = PeachEditorResourceLoadingManager::PeachEditorResourceLoader().GetDrawableResourceLoadingQueue();

        pm_OpenGLRenderer = new PeachCore::OpenGLRenderer(pm_CurrentWindow, false);

        InternalLogManager::InternalRenderingLogger().Debug("RenderingManager successfully initialized >w<", "RenderingManager");

        pm_HasBeenInitialized = true;

        CreatePeachEConsole();
        SetupRenderTexture(fp_Width, fp_Height);

        return pm_CommandQueue; //returns one and only one ptr to whoever initializes RenderingManager, this is meant only for the main thread
    }

    void
        PeachEditorRenderingManager::ProcessCommands()
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
        PeachEditorRenderingManager::ProcessLoadedResourcePackages()
    {
        unique_ptr<PeachCore::LoadedResourcePackage> ResourcePackage;
        while (pm_LoadedResourceQueue->PopLoadedResourceQueue(ResourcePackage))
        {
            visit(
                PeachCore::overloaded
                {
                [&](PeachCore::TextureData& fp_TextureByteData)
                {
                    // Handle creation logic here
                    //stbi_image_free(fp_TextureByteData.get()); //unload texture data
                },
                [](auto&&) 
                {
                    // Default handler for any unhandled types
                    InternalLogManager::InternalRenderingLogger().Warn("Unhandled type in variant for ProcessLoadedResourcePackage", "PeachEditorRenderingManager");
                }
                }, ResourcePackage.get()->ResourceData);
        }
    }

    void PeachEditorRenderingManager::RenderFrame()
    {
        //ProcessLoadedResourcePackages(); //move all loaded objects into memory here if necessary
        //ProcessCommands(); //process all updates
                // Main loop that continues until the window is closed

        bool f_ShouldConsoleBeOpen = true;

        //ImGuiIO& f_ImGuiIO = ImGui::GetIO();
        //f_ImGuiIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking

        //sf::Clock f_DeltaClock;

        //PeachCore::RenderingManager::Renderer().Initialize();
        //PeachCore::ResourceLoadingManager::ResourceLoader().LoadTextureFromSpecifiedFilePath("D:/Game Development/Peach-E/First Texture.png");

        int f_MainMenuBarYOffSet = 0; //used for tracking the total y size of the mainmenu bar

        //also makings only one call to the windowsize each loop, just feels cleaner and easier to read
        int f_CurrentAvailableWindowSpaceX = 0; //adjusts for the main menu bar offset
        int f_CurrentAvailableWindowSpaceY = 0;

        ImGuiWindowFlags file_system_window_flags
            = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

        ImGuiWindowFlags viewport_window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

        ImGuiWindowFlags scene_tree_view_window_flags
            = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

        ImGuiWindowFlags console_window_flags
            = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

        //////////////////////////////////////////////
        // MAIN RENDER LOOP FOR THE EDITOR
        //////////////////////////////////////////////

        SDL_GLContext* gl_context = pm_OpenGLRenderer->GetCurrentOpenGLContext(); //gl_context life span < OpenGLRenderer ALWAYS

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        // Setup ImGui binding
        ImGui_ImplSDL2_InitForOpenGL(pm_CurrentWindow, gl_context);
        ImGui_ImplOpenGL3_Init("#version 130");

        ImGui::StyleColorsDark();

        bool f_IsProgramRuntimeOver = false;

        while (!f_IsProgramRuntimeOver)
        {
            Clear(); //clears screen and beings drawing

            //////////////////////////////////////////////
            // Input Polling for Imgui/SFML
            //////////////////////////////////////////////

            SDL_Event event;

            while (SDL_PollEvent(&event))
            {
                ImGui_ImplSDL2_ProcessEvent(&event);

                if (event.type == SDL_QUIT)
                {
                    f_IsProgramRuntimeOver = true;
                }

                // Handle window resize
                //if (event.type == sf::Event::Resized) 
                //{
                //    // Recreate the render texture with new dimensions
                //    ResizeRenderTexture(event.size.width, event.size.height);
                //    pm_Camera2D->SetSize(event.size.width, event.size.height); // Adjust camera as well if necessary
                //    pm_Camera2D->SetCenter(event.size.width * 0.50f, event.size.height * 0.50f);
                //    pm_CurrentWindow->setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
                //}
            }

            //////////////////////////////////////////////
            // Update Imgui UI
            //////////////////////////////////////////////

           // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            //////////////////////////////////////////////
            // Menu Bar Setup
            //////////////////////////////////////////////

            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("New Project"))
                    {
                        // New action
                    }
                    if (ImGui::MenuItem("Open Project"))
                    {
                        // Open action
                    }
                    if (ImGui::MenuItem("Save Project"))
                    {
                        // Save action
                    }
                    if (ImGui::MenuItem("Save Project as. . ."))
                    {
                        // Save action
                    }
                    if (ImGui::MenuItem("Exit")) //THIS CRASHES EVERYTHING BUT I GUESS THAT IS A WAY TO CLOSE STUFF
                    {
                        Clear(); // IMPORTANT: NEEDA EXPLICITLY ENDDRAWING() BEFORE CALLING CLOSEWINDOW!

                        if(m_IsSceneCurrentlyRunning)
                        {
                            PeachCore::RenderingManager::Renderer().ForceQuit();
                            m_IsSceneCurrentlyRunning = false;
                        }

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
                                
                                PeachEditorRenderingManager::PeachEngineRenderer().m_IsSceneCurrentlyRunning = false;
                            });
                        T_CurrentSceneRunnerThread.detach();
                        m_IsSceneCurrentlyRunning = true;
                    }
                    if (ImGui::MenuItem("Force Quit Peach-E Project") && m_IsSceneCurrentlyRunning) //only works if a valid scene instance is running
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

                if (ImGui::BeginMenu("Editor"))
                {
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

                if (ImGui::BeginMenu("Tools"))
                {
                    if (ImGui::MenuItem("Open Terminal"))
                    {
                        // New action
                    }
                    if (ImGui::MenuItem("Run Stress Test"))
                    {
                        StessTest(1000, 50);
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

            f_MainMenuBarYOffSet = ImGui::GetItemRectSize().y;
            //gets size of window in terms of pixels not screen coordinates
            SDL_GL_GetDrawableSize(pm_CurrentWindow, &f_CurrentAvailableWindowSpaceX, &f_CurrentAvailableWindowSpaceY);

            f_CurrentAvailableWindowSpaceY -= f_MainMenuBarYOffSet;

            //////////////////////////////////////////////
           // Render Texture Setup
           //////////////////////////////////////////////

            //if (pm_ViewportRenderTexture)
            //{
            //    pm_ViewportRenderTexture->clear(sf::Color(128, 128, 128)); // Clear with grey >w<, or any color you need

            //    pm_ViewportRenderTexture->display(); // Updates the texture with what has been drawn

            //    // Initialize Dockspace
            //    ImGuiID dockspace_id = ImGui::GetID("ViewportDockspace");
            //    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

            //    //ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.5f); // Adjust border size
            //    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));  // Set padding to zero for current window
            //    //ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // Set border color to white


            //    ImGui::SetNextWindowPos(ImVec2(0, f_MainMenuBarYOffSet));
            //    ImGui::SetNextWindowSize(ImVec2(f_CurrentAvailableWindowSpaceX * 0.70f, f_CurrentAvailableWindowSpaceY * 0.60f)); //SIZE OF IMGUI VIEWPORT

            //    if (ImGui::Begin("Viewport", nullptr, viewport_window_flags))
            //    {
            //        ImGui::Image(*pm_ViewportRenderTexture);
            //    }

            //    ImGui::End();
            //    ImGui::PopStyleVar(); // Pop border size
            //    //ImGui::PopStyleColor(); // Pop border color
            //}

            //ImVec2 viewportSize = ImGui::GetContentRegionAvail();
            //ImVec2 viewportPos = ImGui::GetCursorScreenPos();

            //// Get mouse position in viewport
            //ImVec2 mousePos = ImGui::GetMousePos();
            //bool isMouseInViewport = ImGui::IsMouseHoveringRect(viewportPos, ImVec2(viewportPos.x + viewportSize.x, viewportPos.y + viewportSize.y));

            //if (isMouseInViewport)
            //{
            //    //// Convert ImGui mouse coordinates to viewport coordinates
            //    //Vector2 mousePosInViewport = { mousePos.x - viewportPos.x, mousePos.y - viewportPos.y };

            //    //// Handle drag and drop or selection
            //    //if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            //    //    // Handle mouse click
            //    //}
            //    //if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            //    //    // Handle mouse drag
            //    //}
            //}

            //////////////////////////////////////////////
            // Virtual File System Setup
            //////////////////////////////////////////////

            //start at the (x,y) that is located at the bottom right corner of the viewport
            ImGui::SetNextWindowPos(ImVec2(f_CurrentAvailableWindowSpaceX * 0.70f, f_CurrentAvailableWindowSpaceY * 0.60f + f_MainMenuBarYOffSet));
            //SIZE OF FILE SYSTEM VIEW PANEL
            ImGui::SetNextWindowSize(ImVec2(f_CurrentAvailableWindowSpaceX * 0.30f, f_CurrentAvailableWindowSpaceY * 0.40f + f_MainMenuBarYOffSet));

            // Your ImGui code goes here
            ImGui::Begin("File System", nullptr, file_system_window_flags);
            ImGui::Text("Hello PhysFS");
            ImGui::End();

            //////////////////////////////////////////////
            // Scene Tree View Panel
            //////////////////////////////////////////////

            ImGui::SetNextWindowPos(ImVec2(f_CurrentAvailableWindowSpaceX * 0.70f, f_MainMenuBarYOffSet)); //start at the x point thats directly to the right of the viewport
            ImGui::SetNextWindowSize(ImVec2(f_CurrentAvailableWindowSpaceX * 0.30f, f_CurrentAvailableWindowSpaceY * 0.60f)); //SIZE OF SCENE TREE VIEW PANEL

            // Panels
            ImGui::Begin("Hierarchy", nullptr, scene_tree_view_window_flags);
            ImGui::Text("Scene Hierarchy");
            ImGui::End();

            //////////////////////////////////////////////
            // Peach-E Console
            //////////////////////////////////////////////

            ImGui::SetNextWindowPos(ImVec2(0, f_CurrentAvailableWindowSpaceY * 0.60f + f_MainMenuBarYOffSet)); //start at the y point thats right "above" (below in this context) the viewport
            ImGui::SetNextWindowSize(ImVec2(f_CurrentAvailableWindowSpaceX * 0.70f, f_CurrentAvailableWindowSpaceY * 0.40f + f_MainMenuBarYOffSet)); //SIZE OF PEACH-E CONSOLE

            pm_EditorConsole->Draw("PEACH CONSOLE", console_window_flags, &f_ShouldConsoleBeOpen);

            ImGui::Render(); // ends the ImGui content mode. Make all ImGui calls before this

            glViewport(0, 0, static_cast<int>(ImGui::GetIO().DisplaySize.x), static_cast<int>(ImGui::GetIO().DisplaySize.y));
            glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            SDL_GL_SwapWindow(pm_CurrentWindow);
        }

        // cleans up ImGui, OpenGL, and our SDL window context
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(pm_CurrentWindow);
        SDL_Quit();
    }

    // Call this method to setup the render texture
    bool 
        PeachEditorRenderingManager::SetupRenderTexture(const unsigned int fp_Width, const unsigned int fp_Height, bool IsNearestNeighbour) //pass in screen width and size, and it scales it to the desired proportions automatically
    {
 /*       if (pm_ViewportRenderTexture)
        {
            InternalLogManager::InternalRenderingLogger().Warn("Attempted to setup render texture again when a valid instance of pm_ViewportRenderTexture is running", "PeachEditorRenderingManager");
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
            InternalLogManager::InternalRenderingLogger().Warn("Failed to create a valid instance of pm_ViewportRenderTexture, setting it back to nullptr", "PeachEditorRenderingManager");
            pm_ViewportRenderTexture = nullptr;
            return false;
        }

        return true;*/
        return true;
    }

    bool 
        PeachEditorRenderingManager::ResizeRenderTexture(const unsigned int fp_Width, const unsigned int fp_Height, bool IsNearestNeighbour)  //pass in screen width and size, and it scales it to the desired proportions automatically
    {
        //if (pm_ViewportRenderTexture->create(fp_Width * 0.70f, fp_Height * 0.60f) && !IsNearestNeighbour)
        //{
        //    pm_ViewportRenderTexture->setSmooth(true);
        //}
        //else if (pm_ViewportRenderTexture->create(fp_Width * 0.70f, fp_Height * 0.60f) && IsNearestNeighbour)
        //{
        //    pm_ViewportRenderTexture->setSmooth(false);
        //}
        //else
        //{
        //    InternalLogManager::InternalRenderingLogger().Warn("Failed to resize a valid instance of pm_ViewportRenderTexture, setting it back to nullptr", "PeachEditorRenderingManager");
        //    pm_ViewportRenderTexture = nullptr;
        //    return false;
        //}

        //return true;
        return true;
    }

    void 
        PeachEditorRenderingManager::RunCurrentScene()
    {

    }

    void 
        PeachEditorRenderingManager::Clear()
    {
        if (pm_CurrentWindow)
        {
            glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
            glClear(GL_COLOR_BUFFER_BIT);
        }
    }

    void 
        PeachEditorRenderingManager::EndFrame()
    {

    }

    void 
        PeachEditorRenderingManager::CreatePeachEConsole()
    {
        pm_EditorConsole = make_unique<PeachEConsole>();
    }

    void 
        PeachEditorRenderingManager::CreateSceneTreeViewPanel()
    {


    }

    string 
        PeachEditorRenderingManager::GetRendererType() 
        const
    {
        return pm_RendererType;
    }

    void 
        PeachEditorRenderingManager::GetCurrentViewPort()
    {

    }

    unsigned int 
        PeachEditorRenderingManager::GetFrameRateLimit() 
        const
    {
        return pm_FrameRateLimit;
    }

    bool 
        PeachEditorRenderingManager::IsVSyncEnabled() 
        const
    {
        return pm_IsVSyncEnabled;
    }

    void 
        PeachEditorRenderingManager::SetVSync(const bool fp_IsEnabled)
    {
        if (fp_IsEnabled)
        {
            pm_IsVSyncEnabled = fp_IsEnabled;
        }
    }

    void 
        PeachEditorRenderingManager::SetFrameRateLimit(unsigned int fp_Limit)
    {
        pm_FrameRateLimit = fp_Limit;
    }
}