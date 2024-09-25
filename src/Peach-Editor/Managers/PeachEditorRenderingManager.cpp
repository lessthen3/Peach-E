#include "../../include/Peach-Editor/Managers/PeachEditorRenderingManager.h"
/*
    This class is used to manage the render thread, and queue/unqueue objects safely
    Copyright(c) 2024-present Ranyodh Singh Mandur.

*/

namespace PeachEditor {

    PeachEditorRenderingManager::~PeachEditorRenderingManager()
    {
        //Shutdown();
    }

    void 
        PeachEditorRenderingManager::Shutdown()
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////
        // cleans up ImGui, OpenGL, and our SDL window context
        ImGui_ImplSDL2_Shutdown();
        //ImGui_Implbgfx_Shutdown();
        ImGui::DestroyContext();

        if (pm_PeachRenderer)
        {
            pm_PeachRenderer.reset(nullptr);
        }
        if (pm_MainWindow)
        {
            //WARNING: DELETING THE CURRENT WINDOW AFTER DESTROYING THE SDL WINDOW AND CALLING SDL_QUIT() CAUSES A HEAP MEMORY VIOLATION
            SDL_DestroyWindow(pm_MainWindow);
            pm_MainWindow = nullptr;
        }
    }

    bool 
        PeachEditorRenderingManager::CreateSDLWindow
        (
            const char* fp_WindowTitle, 
            const uint32_t fp_WindowWidth,
            const uint32_t fp_WindowHeight
        )
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) 
        {
            InternalLogManager::InternalRenderingLogger().Fatal("SDL could not initialize! SDL_Error: " + string(SDL_GetError()), "PeachEditorRenderingManager");
            return false;
        }

        pm_MainWindow = 
            SDL_CreateWindow
        (
            fp_WindowTitle,
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            fp_WindowWidth, fp_WindowHeight,
            SDL_WINDOW_SHOWN
        );

        if (not pm_MainWindow) 
        {
            InternalLogManager::InternalRenderingLogger().Fatal("Window could not be created! SDL_Error: " + string(SDL_GetError()), "PeachEditorRenderingManager");
            SDL_Quit();
            return false;
        }

        return true;
    }

    //creates a window and opengl context, enables sfml 2d graphics and such as well, returns the command queue for thread safe control
    shared_ptr<PeachCore::CommandQueue>
        PeachEditorRenderingManager::InitializeQueues()
    {
        if (pm_AreQueuesInitialized)
        {
            InternalLogManager::InternalRenderingLogger().Warn("RenderingManager queues have already been initialized.", "PeachEditorRenderingManager");
            return nullptr;
        }

        pm_CommandQueue = make_shared<PeachCore::CommandQueue>();
        pm_LoadedResourceQueue = PeachEditorResourceLoadingManager::PeachEditorResourceLoader().GetDrawableResourceLoadingQueue();

        //pm_PeachRenderer = make_unique<PeachCore::PeachRenderer>(pm_MainWindow, false);

        InternalLogManager::InternalRenderingLogger().Debug("PeachEditorRenderingManager successfully initialized >w<", "PeachEditorRenderingManager");

        pm_AreQueuesInitialized = true;

        //SetupRenderTexture(fp_Width, fp_Height);

        return pm_CommandQueue; //returns one and only one ptr to whoever initializes RenderingManager, this is meant only for the main thread
    }

    bool 
        PeachEditorRenderingManager::InitializeOpenGL()
    {
        if (pm_IsRenderingInitialized)
        {
            InternalLogManager::InternalRenderingLogger().Warn("PeachEditorRenderingManager tried to initialize OpenGL when rendering has already been initialized", "PeachEditorRenderingManager");
            return false;
        }

        if (not pm_AreQueuesInitialized)
        {
            InternalLogManager::InternalRenderingLogger().Warn("PeachEditorRenderingManager tried to initialize OpenGL before initializing command/loading queues!", "PeachEditorRenderingManager");
            return false;
        }

        if (not pm_MainWindow)
        {
            InternalLogManager::InternalRenderingLogger().Warn("PeachEditorRenderingManager tried to initialize OpenGL before creating the main window!", "PeachEditorRenderingManager");
            return false;
        }

        int f_WindowWidth, f_WindowHeight;
        SDL_GetWindowSize(pm_MainWindow, &f_WindowWidth, &f_WindowHeight);

        SDL_SysWMinfo wmi;
        SDL_VERSION(&wmi.version);

        if (not SDL_GetWindowWMInfo(pm_MainWindow, &wmi)) 
        {
            cerr << "Unable to get window info: " << SDL_GetError() << endl;
            throw runtime_error("Failed to get window manager info.");
        }
        else
        {
            InternalLogManager::InternalRenderingLogger().Debug("SDL window info successfully read", "PeachEditorRenderingManager");
        }

        bgfx::Init bgfxInit;

        #if defined(_WIN32) || defined(_WIN64)
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
            InternalLogManager::InternalRenderingLogger().Fatal("BGFX failed to initialize. RIP", "PeachRenderer");
            throw runtime_error("BGFX initialization failed");
        }
        else
        {
            InternalLogManager::InternalRenderingLogger().Debug("BGFX initialized properly", "PeachRenderer");
        }

        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
        bgfx::setViewRect(0, 0, 0, f_WindowWidth, f_WindowHeight);

        bgfx::frame();

        peach_editor = &PeachEditorManager::PeachEditor();
        rendering_logger = &InternalLogManager::InternalRenderingLogger();

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        //f_ImGuiIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking

        // Setup ImGui binding
        ImGui_Implbgfx_Init(255);
        ImGui_ImplSDL2_InitForOpenGL(pm_MainWindow, nullptr);

        ImGui::StyleColorsDark();

        pm_FileSystemWindowFlags
            = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

        pm_ViewportWindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

        pm_SceneTreeViewWindowFlags
            = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

        pm_PeachConsoleWindowFlags
            = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

        pm_IsRenderingInitialized = true;
            
        return true;
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

    void 
        PeachEditorRenderingManager::RenderFrame
        (
            bool* fp_IsProgramRuntimeOver
        )
    {
        if (not pm_IsRenderingInitialized)
        {
            //rendering_logger isn't initialized yet if rendering hasn't been initialized yet so we use the full singleton call here instead for safety
            InternalLogManager::InternalRenderingLogger().Fatal("Tried to render frame before rendering was initialized inside of PeachEditorRenderingManager", "PeachEditorRenderingManager");
            throw runtime_error("Tried to render frame before rendering was initialized inside of PeachEditorRenderingManager");
        }

        bool f_ShouldConsoleBeOpen = true;

        int f_MainMenuBarYOffSet = 0; //used for tracking the total y size of the mainmenu bar

        //also makings only one call to the windowsize each loop, just feels cleaner and easier to read
        int f_CurrentAvailableWindowSpaceX = 0; //adjusts for the main menu bar offset
        int f_CurrentAvailableWindowSpaceY = 0;

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
                *fp_IsProgramRuntimeOver = true;
            }

            // Handle window resize
            //if (event.type == sf::Event::Resized) 
            //{
            //    // Recreate the render texture with new dimensions
            //    ResizeRenderTexture(event.size.width, event.size.height);
            //    pm_Camera2D->SetSize(event.size.width, event.size.height); // Adjust camera as well if necessary
            //    pm_Camera2D->SetCenter(event.size.width * 0.50f, event.size.height * 0.50f);
            //    pm_MainWindow->setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
            //}
        }

        //////////////////////////////////////////////
        // Update Imgui UI
        //////////////////////////////////////////////

        // Start the Dear ImGui frame
        ImGui_Implbgfx_NewFrame();
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
                        m_IsSceneCurrentlyRunning = false;
                        PeachCore::RenderingManager::Renderer().ForceQuit();
                    }

                    *fp_IsProgramRuntimeOver = true;
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
                if (ImGui::MenuItem("Run Peach-E Project") and not m_IsSceneCurrentlyRunning)
                {
                    // Run the game in a new window
                    thread T_CurrentSceneRunnerThread([]()
                        {
                            PeachCore::RenderingManager::Renderer().CreateSDLWindow("Peach Game", 800, 600);
                            PeachCore::RenderingManager::Renderer().CreatePeachRenderer();

                            PeachCore::RenderingManager::Renderer().RenderFrame();
                                
                            PeachEditorRenderingManager::PeachEngineRenderer().m_IsSceneCurrentlyRunning = false;
                        });
                    T_CurrentSceneRunnerThread.detach();
                    m_IsSceneCurrentlyRunning = true;
                }
                if (ImGui::MenuItem("Force Quit Peach-E Project") and m_IsSceneCurrentlyRunning) //only works if a valid scene instance is running
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
                    //StessTest(1000, 50);
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
        //gets size of window in terms of pixels not screen coordinates, we do this everytime in case the screen resizes
        SDL_GL_GetDrawableSize(pm_MainWindow, &f_CurrentAvailableWindowSpaceX, &f_CurrentAvailableWindowSpaceY);

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

        //    if (ImGui::Begin("Viewport", nullptr, pm_ViewportWindowFlags))
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
        ImGui::Begin("File System", nullptr, pm_FileSystemWindowFlags);
        //static unordered_map<string,filesystem::file_time_type> fileState = peach_editor->GetCurrentDirectoryState(directory);

        RenderFileBrowser("../../");
        ImGui::End();

        //////////////////////////////////////////////
        // Scene Tree View Panel
        //////////////////////////////////////////////

        ImGui::SetNextWindowPos(ImVec2(f_CurrentAvailableWindowSpaceX * 0.70f, f_MainMenuBarYOffSet)); //start at the x point thats directly to the right of the viewport
        ImGui::SetNextWindowSize(ImVec2(f_CurrentAvailableWindowSpaceX * 0.30f, f_CurrentAvailableWindowSpaceY * 0.60f)); //SIZE OF SCENE TREE VIEW PANEL

        // Panels
        ImGui::Begin("Hierarchy", nullptr, pm_SceneTreeViewWindowFlags);
        ImGui::Text("Scene Hierarchy");
        ImGui::End();

        //////////////////////////////////////////////
        // Peach-E Console
        //////////////////////////////////////////////

        ImGui::SetNextWindowPos(ImVec2(0, f_CurrentAvailableWindowSpaceY * 0.60f + f_MainMenuBarYOffSet)); //start at the y point thats right "above" (below in this context) the viewport
        ImGui::SetNextWindowSize(ImVec2(f_CurrentAvailableWindowSpaceX * 0.70f, f_CurrentAvailableWindowSpaceY * 0.40f + f_MainMenuBarYOffSet)); //SIZE OF PEACH-E CONSOLE

        rendering_logger->GetConsole()->Draw("PEACH CONSOLE", pm_PeachConsoleWindowFlags, &f_ShouldConsoleBeOpen);

        ImGui::Render(); // ends the ImGui content mode. Make all ImGui calls before this

        // glViewport(0, 0, static_cast<int>(ImGui::GetIO().DisplaySize.x), static_cast<int>(ImGui::GetIO().DisplaySize.y));

        ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());

        bgfx::touch(0);
        bgfx::frame();
    }

    // Call this method to setup the render texture
    bool 
        PeachEditorRenderingManager::SetupRenderTexture(const uint32_t fp_Width, const uint32_t fp_Height, bool IsNearestNeighbour) //pass in screen width and size, and it scales it to the desired proportions automatically
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
        PeachEditorRenderingManager::ResizeRenderTexture(const uint32_t fp_Width, const uint32_t fp_Height, bool IsNearestNeighbour)  //pass in screen width and size, and it scales it to the desired proportions automatically
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

    // Helper function to list directories and files
    void 
        PeachEditorRenderingManager::RenderFileBrowser(const fs::path& fp_TopLevelDirectoryPath)
    {
        // Variable to keep track of open directories
        static unordered_set<string> f_CurrentlyOpenDirectories;

        for (const auto& entry : fs::directory_iterator(fp_TopLevelDirectoryPath))
        {
            string f_FileName = entry.path().filename().string();

            if (entry.is_directory()) 
            {
                // Create a unique ID for the tree node using the path
                string uniqueID = "##" + entry.path().string();

                bool isOpen = f_CurrentlyOpenDirectories.find(entry.path().string()) != f_CurrentlyOpenDirectories.end();

                if (ImGui::TreeNodeEx((f_FileName + uniqueID).c_str(), isOpen ? ImGuiTreeNodeFlags_DefaultOpen : 0))
                {
                    if (ImGui::IsItemClicked()) 
                    {
                        // Toggle the directory's open state
                        if (isOpen) 
                        {
                            f_CurrentlyOpenDirectories.erase(entry.path().string());
                        }
                        else 
                        {
                            f_CurrentlyOpenDirectories.insert(entry.path().string());
                        }
                    }

                    if (isOpen) 
                    {
                        RenderFileBrowser(entry.path());  // Recursively render subdirectories
                    }

                    ImGui::TreePop();
                }
            }
            else
            {
                // Files are displayed with the ability to select
                ImGui::PushID(f_FileName.c_str());  // Ensure unique ID for selectable

                if (ImGui::Selectable(f_FileName.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
                {
                    if (ImGui::IsMouseDoubleClicked(0))
                    {
                        // Handle double-click (e.g., open or edit the file)
                        HandleFileAction(entry.path());
                    }
                }

                ImGui::PopID();
            }
        }
    }

    // Example handler for file actions
    void 
        PeachEditorRenderingManager::HandleFileAction(const fs::path& filePath)
    {
        // Implement your file handling logic here, e.g., open a dialog, start an editor, etc.
        std::cout << "File selected: " << filePath << std::endl;
    }

    void 
        PeachEditorRenderingManager::RunCurrentScene()
    {

    }

    void 
        PeachEditorRenderingManager::Clear()
    {

    }

    void 
        PeachEditorRenderingManager::EndFrame()
    {

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

    uint32_t 
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
        PeachEditorRenderingManager::SetFrameRateLimit(uint32_t fp_Limit)
    {
        pm_FrameRateLimit = fp_Limit;
    }
}