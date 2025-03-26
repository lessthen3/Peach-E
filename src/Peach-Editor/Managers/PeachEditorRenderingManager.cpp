#include "../../include/Peach-Editor/Managers/PeachEditorRenderingManager.h"
/*
    This class is used to manage the render thread, and queue/unqueue objects safely
    Copyright(c) 2024-present Ranyodh Singh Mandur.

*/

namespace PeachEditor {

    PeachEditorRenderingManager::~PeachEditorRenderingManager()
    {
    }

    void 
        PeachEditorRenderingManager::Shutdown()
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////
        // cleans up ImGui, OpenGL, and our SDL window context
        nk_sdl_shutdown();

        //if (pm_MainWindow)
        //{
        //    //WARNING: DELETING THE CURRENT WINDOW AFTER DESTROYING THE SDL WINDOW AND CALLING SDL_QUIT() CAUSES A HEAP MEMORY VIOLATION
        //    SDL_DestroyWindow(pm_MainWindow);
        //    pm_MainWindow = nullptr;
        //}

        //SDL_GL_DestroyContext(pm_OpenGLContext);
    }

    bool 
        PeachEditorRenderingManager::InitializeLogger
        (
            const string& fp_LogOutputDirectory,
            shared_ptr<PC::Console> fp_EditorConsole
        )
    {
        //////////////////////////////////////////////
        // Initialize Logger
        //////////////////////////////////////////////
        rendering_logger = make_shared<PC::LogManager>();
        if (not rendering_logger->Initialize(fp_LogOutputDirectory, "PeachEditorRenderingManager", fp_EditorConsole))
        {
            return false;
        }
        rendering_logger->LogAndPrint("PeachEditorRenderingLogger successfully initialized", "PeachEditorRenderingManager", PeachCore::LogManager::LogLevel::Debug, "render_thread");

        return true;
    }

    //creates a window and opengl context, enables sfml 2d graphics and such as well, returns the command queue for thread safe control
    shared_ptr<PeachCore::CommandQueue>
        PeachEditorRenderingManager::InitializeQueues()
    {
        if (pm_AreQueuesInitialized)
        {
            rendering_logger->LogAndPrint("RenderingManager queues have already been initialized.", "PeachEditorRenderingManager", PeachCore::LogManager::LogLevel::Warning, "render_thread");
            return nullptr;
        }

        pm_CommandQueue = make_shared<PeachCore::CommandQueue>();
        pm_LoadedResourceQueue = PeachEditorResourceLoadingManager::PeachEditorResourceLoader().GetDrawableResourceLoadingQueue();

        //pm_PeachRenderer = make_unique<PeachCore::PeachRenderer>(pm_MainWindow, false);

        rendering_logger->LogAndPrint("PeachEditorRenderingManager successfully initialized >w<", "PeachEditorRenderingManager", PeachCore::LogManager::LogLevel::Debug, "render_thread");

        pm_AreQueuesInitialized = true;

        //SetupRenderTexture(fp_Width, fp_Height);

        return pm_CommandQueue; //returns one and only one ptr to whoever initializes RenderingManager, this is meant only for the main thread
    }

    bool 
        PeachEditorRenderingManager::InitializeOpenGL()
    {
        if (pm_IsRenderingInitialized)
        {
            rendering_logger->LogAndPrint("PeachEditorRenderingManager tried to initialize OpenGL when rendering has already been initialized", "PeachEditorRenderingManager", PeachCore::LogManager::LogLevel::Warning, "render_thread");
            return false;
        }

        if (not pm_AreQueuesInitialized)
        {
            rendering_logger->LogAndPrint("PeachEditorRenderingManager tried to initialize OpenGL before initializing command/loading queues!", "PeachEditorRenderingManager", PeachCore::LogManager::LogLevel::Warning, "render_thread");
            return false;
        }

        if (not pm_MainWindow)
        {
            rendering_logger->LogAndPrint("PeachEditorRenderingManager tried to initialize OpenGL before creating the main window!", "PeachEditorRenderingManager", PeachCore::LogManager::LogLevel::Warning, "render_thread");
            return false;
        }

        // Create an OpenGL context associated with the window
        pm_EditorRenderer = make_unique<PeachCore::PeachRenderer>(pm_MainWindow, rendering_logger, true);

        if (glewInit() != GLEW_OK)
        {
            rendering_logger->LogAndPrint("Failed to create GLEW context: OWO", "PeachEditorRenderingManager", PeachCore::LogManager::LogLevel::Fatal, "render_thread");
            SDL_DestroyWindow(pm_MainWindow);
            return false;
        }

        rendering_logger->LogAndPrint("GLEW initialized properly", "PeachEditorRenderingManager", PeachCore::LogManager::LogLevel::Debug, "render_thread");

        ////////////////////////////////////////////////
        // Setup Nuklear GUI
        ////////////////////////////////////////////////

        pm_NuklearCtx = nk_sdl_init(pm_MainWindow);

        struct nk_font_atlas* mf_FontAtlas;
        nk_sdl_font_stash_begin(&mf_FontAtlas);

        string f_DesiredFontDirectory = static_cast<string>(PHYSFS_getWriteDir()) + "/fonts/ComicSansMS.ttf";

        struct nk_font* mf_ComicSans = nk_font_atlas_add_from_file(mf_FontAtlas, f_DesiredFontDirectory.c_str(), 18, 0);

        nk_sdl_font_stash_end();

        if (mf_ComicSans)
        {
            nk_style_set_font(pm_NuklearCtx, &(mf_ComicSans->handle));
        }

        pm_BackgroundColour = { 0.10f, 0.18f, 0.24f, 1.0f };

        ////////////////////////////////////////////////
        // Create Viewport
        ////////////////////////////////////////////////

        int f_CurrentWindowWidth, f_CurrentWindowHeight;

        SDL_GetWindowSizeInPixels(pm_MainWindow, &f_CurrentWindowWidth, &f_CurrentWindowHeight);

        pm_Viewport = Viewport();
        pm_Viewport.SetupViewport(400, 200, pm_EditorRenderer.get(), rendering_logger);

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
                    //rendering_logger->LogAndPrint("Unhandled type in variant for ProcessLoadedResourcePackage", "PeachEditorRenderingManager", LogManager::LogLevel::Warning);
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
            rendering_logger->LogAndPrint("Tried to render frame before rendering was initialized inside of PeachEditorRenderingManager", "PeachEditorRenderingManager", PeachCore::LogManager::LogLevel::Fatal, "render_thread");
            throw runtime_error("Tried to render frame before rendering was initialized inside of PeachEditorRenderingManager");
        }

        if (not fp_IsProgramRuntimeOver)
        {
            rendering_logger->LogAndPrint("Tried to pass nullptr bool to RenderFrame inside of PeachEditorRenderingManager", "PeachEditorRenderingManager", PeachCore::LogManager::LogLevel::Fatal, "render_thread");
            throw runtime_error("Tried to pass nullptr bool to RenderFrame");
        }

        //////////////////////////////////////////////////
        //// Clear Screen
        //////////////////////////////////////////////////
        glClearColor(pm_ClearColour.x, pm_ClearColour.y, pm_ClearColour.z, pm_ClearColour.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //PLEASE GOD DO NOT MOVE THIS HOLY FUCK

        //adjusts for the main menu bar offset
        //also makings only one call to the windowsize each loop, just feels cleaner and easier to read
        int f_CurrentWindowHeight, f_CurrentWindowWidth;

        SDL_GetWindowSizeInPixels(pm_MainWindow, &f_CurrentWindowWidth, &f_CurrentWindowHeight);

        int f_MainMenuBarHeight = static_cast<int>(f_CurrentWindowHeight * MAIN_MENU_BAR_SCALE);
        int f_MainMenuBarWidth = f_CurrentWindowWidth;

        f_MainMenuBarHeight = clamp(f_MainMenuBarHeight, 30, 40);

        //f_CurrentWindowHeight -= f_MainMenuBarHeight; //subtract menu bar since we want the available 
        glViewport(0, 0, f_CurrentWindowWidth, f_CurrentWindowHeight);

        ////////////////////////////////////////////////
        // Input Handling
        ////////////////////////////////////////////////
        SDL_Event f_Event;
        nk_input_begin(pm_NuklearCtx);
        while (SDL_PollEvent(&f_Event))
        {
            if (f_Event.window.windowID == SDL_GetWindowID(pm_MainWindow) and f_Event.window.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
            {
                *fp_IsProgramRuntimeOver = false;
            }
            if (pm_GameInstanceWindow and f_Event.window.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
            {
                if (f_Event.window.windowID == SDL_GetWindowID(pm_GameInstanceWindow))
                {
                    m_IsSceneCurrentlyRunning = false;
                    pm_GameInstanceWindow = nullptr;
                }
            }

            nk_sdl_handle_event(&f_Event);
        }
        nk_sdl_handle_grab();
        nk_input_end(pm_NuklearCtx);

        ////////////////////////////////////////////////
        // Draw GUI
        ////////////////////////////////////////////////
        if 
            (
                nk_begin
                (
                    pm_NuklearCtx, 
                    "Colour Picker", 
                    nk_rect(50, 50, 200, 100),
                    NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE
                )
            )
        {
            nk_layout_row_dynamic(pm_NuklearCtx, 25, 1);

            if (nk_combo_begin_color(pm_NuklearCtx, nk_rgb_cf(pm_BackgroundColour), nk_vec2(nk_widget_width(pm_NuklearCtx), 400))) 
            {
                nk_layout_row_dynamic(pm_NuklearCtx, 120, 1);
                pm_BackgroundColour = nk_color_picker(pm_NuklearCtx, pm_BackgroundColour, NK_RGBA);
                nk_layout_row_dynamic(pm_NuklearCtx, 25, 1);
                pm_BackgroundColour.r = nk_propertyf(pm_NuklearCtx, "#R:", 0, pm_BackgroundColour.r, 1.0f, 0.01f, 0.005f);
                pm_BackgroundColour.g = nk_propertyf(pm_NuklearCtx, "#G:", 0, pm_BackgroundColour.g, 1.0f, 0.01f, 0.005f);
                pm_BackgroundColour.b = nk_propertyf(pm_NuklearCtx, "#B:", 0, pm_BackgroundColour.b, 1.0f, 0.01f, 0.005f);
                pm_BackgroundColour.a = nk_propertyf(pm_NuklearCtx, "#A:", 0, pm_BackgroundColour.a, 1.0f, 0.01f, 0.005f);
                nk_combo_end(pm_NuklearCtx);
            }
        }
        nk_end(pm_NuklearCtx);

        ////////////////////////////////////////////////
        // Menubar
        ////////////////////////////////////////////////

        //nk_style_push_style_item(pm_NuklearCtx, )

        if (nk_begin(pm_NuklearCtx, "Menu_Bar", nk_rect(0, 0, f_CurrentWindowWidth, f_MainMenuBarHeight), NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR))
        {
            nk_menubar_begin(pm_NuklearCtx);

            // Menu label, must match with nk_menu_begin_label call
            nk_layout_row_begin(pm_NuklearCtx, NK_STATIC, 25, NUMBER_OF_HORIZONTAL_MAIN_MENU_BAR_ELEMENTS);
            nk_layout_row_push(pm_NuklearCtx, 45);

            if (nk_menu_begin_label(pm_NuklearCtx, "File", NK_TEXT_LEFT, nk_vec2(static_cast<float>(f_CurrentWindowWidth/4), static_cast<float>(f_CurrentWindowHeight /3))))
            {
                nk_layout_row_dynamic(pm_NuklearCtx, 30, 1);

                if (nk_menu_item_label(pm_NuklearCtx, "Open Project", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("Open menu item clicked\n");
                }
                if (nk_menu_item_label(pm_NuklearCtx, "Save Everything", NK_TEXT_LEFT))
                {
                    // Handle save action
                    cout << ("Save menu item clicked\n");
                }
                if (nk_menu_item_label(pm_NuklearCtx, "Save as", NK_TEXT_LEFT))
                {
                    // Handle save action
                    cout << ("Save as menu item clicked\n");
                }
                if (nk_menu_item_label(pm_NuklearCtx, "New Project", NK_TEXT_LEFT))
                {
                    cout << ("New Project menu item clicked\n");
                }
                if (nk_menu_item_label(pm_NuklearCtx, "Exit", NK_TEXT_LEFT))
                {
                    *fp_IsProgramRuntimeOver = false;
                    m_IsSceneCurrentlyRunning = false;
                }

                nk_menu_end(pm_NuklearCtx);
            }
            if (nk_menu_begin_label(pm_NuklearCtx, "Edit", NK_TEXT_LEFT, nk_vec2(static_cast<float>(f_CurrentWindowWidth / 4), static_cast<float>(f_CurrentWindowHeight / 3))))
            {
                nk_layout_row_dynamic(pm_NuklearCtx, 30, 1);

                if (nk_menu_item_label(pm_NuklearCtx, "Undo", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("Undo menu item clicked\n");
                }
                if (nk_menu_item_label(pm_NuklearCtx, "Redo", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("Redo menu item clicked\n");
                }
                if (nk_menu_item_label(pm_NuklearCtx, "Show Action History", NK_TEXT_LEFT))
                {
                    // Handle save action
                    cout << ("Show Action History menu item clicked\n");
                }
                if (nk_menu_item_label(pm_NuklearCtx, "Show Clipboard History", NK_TEXT_LEFT))
                {
                    cout << ("Show Clipboard History menu item clicked\n");
                }

                nk_menu_end(pm_NuklearCtx);
            }
            if (nk_menu_begin_label(pm_NuklearCtx, "Run", NK_TEXT_LEFT, nk_vec2(static_cast<float>(f_CurrentWindowWidth / 4), static_cast<float>(f_CurrentWindowHeight / 3))))
            {
                nk_layout_row_dynamic(pm_NuklearCtx, 30, 1);

                if (nk_menu_item_label(pm_NuklearCtx, "Run Peach-E Project", NK_TEXT_LEFT) and not m_IsSceneCurrentlyRunning)
                {

                    if (not PeachCore::RenderingManager::Renderer().CreateSDLWindow(&pm_GameInstanceWindow, PeachCore::RendererType::OpenGL, "Peach Game", 800, 600))
                    {
                        //idk do smth idc rn
                    }

                    // Run the game in a new window
                    //THIS IS NOT THREAD SAFE IDK WHY I DID THIS LMFAO
                    thread T_CurrentSceneRunnerThread([]()
                    {
                        auto editor_renderer = &PeachEditor::PeachEditorRenderingManager::PeachEditorRenderer();
                        auto engine_renderer = &PeachCore::RenderingManager::Renderer();

                        SDL_Window* t_GameWindow = editor_renderer->GetGameInstanceWindow();

                        engine_renderer->CreatePeachRenderer(t_GameWindow);

                        PeachCore::PeachRenderer* t_GameInstanceRenderer = engine_renderer->GetPeachRenderer();

                        ////////////////////////////////////////////////
                        // Generate Buffers
                        ////////////////////////////////////////////////

                        vector<float> vertices =
                        {
                            // positions             // texture coords
                            0.5f,  0.5f, 0.0f,   1.0f, 1.0f,   // top right
                            0.5f, -0.5f, 0.0f,   1.0f, 0.0f,   // bottom right
                           -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,   // bottom left
                           -0.5f,  0.5f, 0.0f,   0.0f, 1.0f    // top left 
                        };

                        vector<unsigned int> indices =
                        {  // note that we start from 0!
                            0, 1, 3,   // first triangle
                            1, 2, 3    // second triangle
                        };

                        GLuint vao = t_GameInstanceRenderer->Generate2DBuffers(vertices, indices);

                        ////////////////////////////////////////////////
                        // Shaders
                        ////////////////////////////////////////////////
                        
                        //XXX: NEED TO NUKE THIS NOT THREAD SAFE ONLY USED HERE FOR NOW SINCE EVERYTHING RUNS ON A SINGLE THREAD ATM
                        string f_BaseDir = PHYSFS_getWriteDir(); //WARNING: USED ONLY FOR TESTING NEED THIS TO BE IN RESOURCELOADINGMANAGER

                        PeachCore::ShaderProgram mf_CatShader = PeachCore::ShaderProgram
                        (
                            "Cat_Shader",
                            f_BaseDir + "/shaders/vert.vs",
                            f_BaseDir + "/shaders/frag.fs",
                            engine_renderer->rendering_logger.get()
                        );

                        ////////////////////////////////////////////////
                        // Loading and Registering Texture
                        ////////////////////////////////////////////////

                        string f_TexturePath = f_BaseDir + "/First Texture.png";
                        stbi_set_flip_vertically_on_load(true);

                        int width, height, nrChannels;
                        unsigned char* data = stbi_load(f_TexturePath.c_str(), &width, &height, &nrChannels, 0);

                        GLuint texture = t_GameInstanceRenderer->RegisterTexture("Texture", data, width, height, nrChannels);

                        glm::mat4 mf_Transform = glm::mat4(1.0f);


                        int t_CurrentWindowWidth, t_CurrentWindowHeight;

                        while (editor_renderer->m_IsSceneCurrentlyRunning)
                        {
                            this_thread::sleep_for(chrono::milliseconds(16));

                            SDL_GetWindowSizeInPixels(t_GameWindow, &t_CurrentWindowWidth, &t_CurrentWindowHeight);
                            glViewport(0, 0, t_CurrentWindowWidth, t_CurrentWindowHeight);

                            nk_colorf f_Temp = editor_renderer->pm_BackgroundColour;
                            glm::vec4 f_Colour = { f_Temp.r, f_Temp.g, f_Temp.b, f_Temp.a };

                            mf_Transform = glm::rotate(mf_Transform, static_cast<float>(static_cast<int>(SDL_GetTicks()) % 10000) * 0.00001f, glm::vec3(0.0, 0.0, 1.0));

                            glUseProgram(mf_CatShader.GetProgramID());

                            mf_CatShader.SetUniform("colourUniform", f_Colour);
                            mf_CatShader.SetUniform("transform", mf_Transform);

                            glUseProgram(0);

                            t_GameInstanceRenderer->DrawTexture(mf_CatShader, vao, texture);

                            SDL_GL_SwapWindow(t_GameWindow);

                            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                            glClearColor(0.10f, 0.18f, 0.24f, 1.0f);
                        }

                        //this used to create a bug but doesnt anymore for some reason lmfao
                        engine_renderer->DestroyPeachRenderer(); //IMPORTANT THIS BREAKS THE PROGRAM ITS A THREADING BUG
                        //WE BEED TO SYNCHRONIZE THREADS, CLEANUP RESOURCES IN APPROPRIATE ORDER THEN EXIT MAIN FUNCTION OWO
                        editor_renderer->SetGameInstanceWindow(nullptr);
                    });

                    T_CurrentSceneRunnerThread.detach();
                    m_IsSceneCurrentlyRunning = true;
                }
                if (nk_menu_item_label(pm_NuklearCtx, "Force Quit Peach-E Project", NK_TEXT_LEFT) and m_IsSceneCurrentlyRunning)
                {
                    m_IsSceneCurrentlyRunning = false;
                }

                nk_menu_end(pm_NuklearCtx);
            }

            nk_layout_row_push(pm_NuklearCtx, 60); //adjust the spacing ratio for the elements, since these are longer it looks a bit weirder

            if (nk_menu_begin_label(pm_NuklearCtx, "Project", NK_TEXT_LEFT, nk_vec2(static_cast<float>(f_CurrentWindowWidth / 4), static_cast<float>(f_CurrentWindowHeight / 3))))
            {
                nk_layout_row_dynamic(pm_NuklearCtx, 30, 1);

                if (nk_menu_item_label(pm_NuklearCtx, "Project Settings", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("Project Settings menu item clicked\n");
                }
                if (nk_menu_item_label(pm_NuklearCtx, "Input Map", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("Input Map menu item clicked\n");
                }
                if (nk_menu_item_label(pm_NuklearCtx, "Refresh Project", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("Refresh Project menu item clicked\n");
                }

                nk_menu_end(pm_NuklearCtx);
            }
            if (nk_menu_begin_label(pm_NuklearCtx, "Editor", NK_TEXT_LEFT, nk_vec2(static_cast<float>(f_CurrentWindowWidth / 4), static_cast<float>(f_CurrentWindowHeight / 3))))
            {
                nk_layout_row_dynamic(pm_NuklearCtx, 30, 1);

                if (nk_menu_item_label(pm_NuklearCtx, "Editor Settings", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("Editor Settings menu item clicked\n");
                }
                if (nk_menu_item_label(pm_NuklearCtx, "Editor Theme", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("Editor Theme menu item clicked\n");
                }

                nk_menu_end(pm_NuklearCtx);
            }
            if (nk_menu_begin_label(pm_NuklearCtx, "Tools", NK_TEXT_LEFT, nk_vec2(static_cast<float>(f_CurrentWindowWidth / 4), static_cast<float>(f_CurrentWindowHeight / 3))))
            {
                nk_layout_row_dynamic(pm_NuklearCtx, 30, 1);

                if (nk_menu_item_label(pm_NuklearCtx, "Open Terminal", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("Open Terminal menu item clicked\n");
                }
                if (nk_menu_item_label(pm_NuklearCtx, "Run Stress Test", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("Run Stress Test menu item clicked\n");
                }

                nk_menu_end(pm_NuklearCtx);
            }
            if (nk_menu_begin_label(pm_NuklearCtx, "Plugins", NK_TEXT_LEFT, nk_vec2(static_cast<float>(f_CurrentWindowWidth / 4), static_cast<float>(f_CurrentWindowHeight / 3))))
            {
                nk_layout_row_dynamic(pm_NuklearCtx, 30, 1);

                if (nk_menu_item_label(pm_NuklearCtx, "Load Plugin", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("Load Plugin menu item clicked\n");
                }
                if (nk_menu_item_label(pm_NuklearCtx, "List of Active Plugins", NK_TEXT_LEFT))
                {
                    // Handle open action
                    cout << ("List of Active Plugins menu item clicked\n");
                }

                nk_menu_end(pm_NuklearCtx);
            }

            nk_layout_row_end(pm_NuklearCtx);
            nk_menubar_end(pm_NuklearCtx);
        }
        nk_end(pm_NuklearCtx);

        ////////////////////////////////////////////////
        // Render File Browser
        ////////////////////////////////////////////////

        RenderFileBrowser("../", f_CurrentWindowWidth * 0.85f, f_CurrentWindowHeight*0.70f, f_CurrentWindowWidth*0.15f, f_CurrentWindowHeight*0.30f, pm_NuklearCtx);

        ////////////////////////////////////////////////
        // Render Console
        ////////////////////////////////////////////////

        RenderConsole(pm_NuklearCtx);

        ////////////////////////////////////////////////
        // Render Viewport
        ////////////////////////////////////////////////

        unsigned int mf_ViewportHeight = f_CurrentWindowHeight * 0.60f;
        unsigned int mf_ViewportWidth = f_CurrentWindowWidth * 0.70f;

        glm::vec2 mf_ViewportPosition = glm::vec2(0.0f, f_CurrentWindowHeight - mf_ViewportHeight - f_MainMenuBarHeight); //glViewPort x and y args arent based on the 4th quadrant scheme for some reason lmfao it treats the screen as the first quadrant?_?

        pm_Viewport.RenderViewport(mf_ViewportPosition, mf_ViewportWidth, mf_ViewportHeight);

        ////////////////////////////////////////////////
        // Render Nuklear Context
        ////////////////////////////////////////////////

        nk_sdl_render(NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);

        SDL_GL_SwapWindow(pm_MainWindow);
    }

    void 
        PeachEditorRenderingManager::RenderDirectory
        (
            struct nk_context* ctx, 
            const fs::path& fp_CurrentPath
        )
    {
        for (const auto& entry : fs::directory_iterator(fp_CurrentPath))
        {
            string f_FileName = entry.path().filename().string();
            string f_FullPathName = entry.path().string();

            if (entry.is_directory()) 
            {
                enum nk_collapse_states f_IsOpen = 
                    (pm_CurrentlyOpenDirectories.find(f_FullPathName) != pm_CurrentlyOpenDirectories.end())
                    ? 
                    NK_MAXIMIZED: 
                    NK_MINIMIZED;

                if (nk_tree_state_push(ctx, NK_TREE_NODE, f_FileName.c_str(), &f_IsOpen))
                {
                    RenderDirectory(ctx, entry.path());
                    pm_CurrentlyOpenDirectories.insert(f_FullPathName);
                    nk_tree_pop(ctx);
                }
                else
                {
                    pm_CurrentlyOpenDirectories.erase(f_FullPathName);
                }
            }
            else 
            {
                int isSelected = pm_SelectionState.SelectedFiles.find(f_FullPathName) != pm_SelectionState.SelectedFiles.end() ? 1 : 0;
                
                // Update this every frame before rendering the UI
                bool ctrlPressed = nk_input_is_key_down(&ctx->input, NK_KEY_CTRL);
                bool shiftPressed = nk_input_is_key_down(&ctx->input, NK_KEY_SHIFT);

                if (nk_selectable_label(ctx, f_FileName.c_str(), NK_TEXT_LEFT, &isSelected)) 
                {
                    if (ctrlPressed)
                    {
                        cout << "ctrl pressd \n";
                        // Control key is pressed, add or remove from the selection
                        if (isSelected) 
                        {
                            pm_SelectionState.SelectedFiles.insert(f_FullPathName);
                            pm_SelectionState.LastSelectedItem = f_FullPathName;
                        }
                        else 
                        {
                            pm_SelectionState.SelectedFiles.erase(f_FullPathName);
                        }
                    }
                    else if (shiftPressed and not pm_SelectionState.LastSelectedItem.empty())
                    {
                        // Shift key is pressed, select all items between this item and the last selected item
                        // This requires knowing the order of files which might need additional logic
                        HandleRangeSelection(pm_SelectionState.LastSelectedItem, f_FullPathName);
                    }
                    else 
                    {
                        // No modifier keys, select only this item
                        pm_SelectionState.SelectedFiles.clear();
                        pm_SelectionState.SelectedFiles.insert(f_FullPathName);
                        pm_SelectionState.LastSelectedItem = f_FullPathName;

                        //HANDLE FILE ACTION HERE FUTURE ME ;)
                        HandleFileSelection(f_FullPathName);
                    }
                }
            }
        }
    }

    void
        PeachEditorRenderingManager::HandleRangeSelection
        (
            const string& fp_FirstItemSelected,
            const string& fp_SecondItemSelected
        )
    {
        // This method should calculate the range between the lastSelectedItem and clickedItem
        // and select all items in between.
        // The exact implementation will depend on how you can determine the order of items.
        cout << "Range Selected: " + fp_FirstItemSelected + " -->  " + fp_SecondItemSelected << "\n";
    }

    void
        PeachEditorRenderingManager::HandleFileSelection(const string& fp_FileItemSelected)
    {
        // Implement your file handling logic here, e.g., open a dialog, start an editor, etc.
        cout << "File selected: " << fp_FileItemSelected << endl;
    }


    // Helper function to list directories and files
    void 
        PeachEditorRenderingManager::RenderFileBrowser
        (
            const fs::path& fp_TopLevelDirectoryPath, 
            float x, 
            float y, 
            float width, 
            float height,
            struct nk_context* ctx
        )
    {
        static fs::path current_path = fp_TopLevelDirectoryPath;  // Holds the current directory path

        if (not fs::exists(current_path) || not fs::is_directory(current_path)) 
        {
            current_path = fp_TopLevelDirectoryPath;  // Reset to base directory if the current path is not valid
        }

        struct nk_style_item f_ButtonDefaultStyle = { NK_STYLE_ITEM_COLOR, nk_rgba(45, 55, 159, 255) };
        struct nk_style_item f_ButtonHoverStyle = { NK_STYLE_ITEM_COLOR, nk_rgba(40, 40, 40, 255) };
        struct nk_style_item f_ButtonActiveStyle = { NK_STYLE_ITEM_COLOR, nk_rgba(20, 20, 20, 255) };

        nk_style_push_style_item(ctx, (&ctx->style.button.normal), f_ButtonDefaultStyle); // Transparent normal state
        nk_style_push_style_item(ctx, &ctx->style.button.hover, f_ButtonHoverStyle);  // Darker hover state
        nk_style_push_style_item(ctx, &ctx->style.button.active, f_ButtonActiveStyle); // Even darker active state

        //nk_style_push_color(ctx, &ctx->style.button.border_color, nk_rgba(45, 55, 159, 255)); // No border
        //nk_style_push_vec2(ctx, &ctx->style.button.padding, nk_vec2(0, 0));             // No padding
        nk_style_push_color(ctx, &ctx->style.selectable.text_normal, nk_rgb(100, 100, 255));
        nk_style_push_color(ctx, &ctx->style.selectable.text_hover, nk_rgb(0, 255, 255));
        nk_style_push_color(ctx, &ctx->style.selectable.text_normal_active, nk_rgb(255, 0, 255));

        nk_style_push_style_item(ctx, &ctx->style.selectable.normal, { NK_STYLE_ITEM_COLOR, nk_rgba(50, 50, 50, 255) });
        nk_style_push_style_item(ctx, &ctx->style.selectable.hover, { NK_STYLE_ITEM_COLOR, nk_rgba(50, 50, 150, 255) });
        nk_style_push_style_item(ctx, &ctx->style.selectable.pressed, { NK_STYLE_ITEM_COLOR,nk_rgba(50, 50, 200, 255) });
        nk_style_push_style_item(ctx, &ctx->style.selectable.normal_active, { NK_STYLE_ITEM_COLOR,nk_rgba(50, 50, 200, 255) });
        nk_style_push_style_item(ctx, &ctx->style.selectable.hover_active, { NK_STYLE_ITEM_COLOR,nk_rgba(50, 50, 255, 255) });


        struct nk_rect space = nk_rect(x, y, width, height);

        if (nk_begin(ctx, "File Browser", space, NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_SCROLL_AUTO_HIDE))
        {
            nk_layout_row_dynamic(ctx, 1, 1);  // Dynamic row layout with one column

            // Iterating over each entry in the directory
            RenderDirectory(ctx, current_path);
        }
        nk_end(ctx);

        // Pop styles
        nk_style_pop_style_item(ctx);
        nk_style_pop_style_item(ctx);
        nk_style_pop_style_item(ctx);

        nk_style_pop_color(ctx);
        nk_style_pop_color(ctx);
        nk_style_pop_color(ctx);
        //nk_style_pop_color(ctx);
        //nk_style_pop_vec2(ctx);
        
        nk_style_pop_style_item(ctx);
        nk_style_pop_style_item(ctx);
        nk_style_pop_style_item(ctx);
        nk_style_pop_style_item(ctx);
        nk_style_pop_style_item(ctx);
    }

    void 
        PeachEditorRenderingManager::RenderConsole
        (
            struct nk_context* ctx
        )
    {
        static int activeTab = 0; // 0 = Logs, 1 = Warnings, 2 = Errors

        if (nk_begin(ctx, "Developer Console", nk_rect(50, 50, 600, 400),
            NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE))
        {
            // Tabs for Log Types
            nk_layout_row_static(ctx, 30, 80, 3);
            if (nk_button_label(ctx, "Logs")) activeTab = 0;
            if (nk_button_label(ctx, "Warnings")) activeTab = 1;
            if (nk_button_label(ctx, "Errors")) activeTab = 2;

            // Scrollable Console Output
            nk_layout_row_dynamic(ctx, 250, 1);
            if (nk_group_begin(ctx, "ConsoleOutput", NK_WINDOW_BORDER))
            {
                nk_layout_row_dynamic(ctx, 18, 1);

                std::vector<std::string>* selectedLog = nullptr;
                //if (activeTab == 0) selectedLog = &consoleLogs;
                //else if (activeTab == 1) selectedLog = &consoleWarnings;
                //else selectedLog = &consoleErrors;

                //for (const auto& msg : *selectedLog)
                //{
                //    nk_label(ctx, msg.c_str(), NK_TEXT_LEFT);
                //}

                nk_group_end(ctx);
            }

            // Input Box for Commands
            nk_layout_row_dynamic(ctx, 25, 2);
            //nk_edit_string_zero_terminated(ctx, NK_EDIT_SIMPLE, commandBuffer, sizeof(commandBuffer), nk_filter_default);
            //if (nk_button_label(ctx, "Run"))
            //{
            //    // Add command to logs and clear buffer
            //    consoleLogs.push_back(std::string("> ") + commandBuffer);
            //    std::cout << "Command Entered: " << commandBuffer << std::endl;
            //    memset(commandBuffer, 0, sizeof(commandBuffer));
            //}
        }
        nk_end(ctx);
    }

    void 
        PeachEditorRenderingManager::RunCurrentScene()
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

    uint32_t 
        PeachEditorRenderingManager::GetFrameRateLimit() 
        const
    {
        return pm_FrameRateLimit;
    }

    SDL_Window*&
        PeachEditorRenderingManager::GetMainWindow()
    {
        return pm_MainWindow;
    }

    Viewport*
        PeachEditorRenderingManager::GetViewport()
    {
        return &pm_Viewport;
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
        pm_IsVSyncEnabled = fp_IsEnabled;
    }

    void 
        PeachEditorRenderingManager::SetFrameRateLimit(uint32_t fp_Limit)
    {
        pm_FrameRateLimit = fp_Limit;
    }
} // namespace

namespace PeachEditor {

    void
        Viewport::SetupViewport
        (
            const unsigned int fp_Width,
            const unsigned int fp_Height,
            PC::PeachRenderer* fp_Renderer,
            shared_ptr<PC::LogManager> fp_EditorRenderingLogger
        )
    {
        pm_CurrentViewportWidth = fp_Width;
        pm_CurrentViewportHeight = fp_Height;

        ////////////////////////////////////////////////
        // Get Reference to Current Renderer
        ////////////////////////////////////////////////

        if (not fp_Renderer)
        {
            //handle error here
            return;
        }
        pm_Render = fp_Renderer;

        editor_rendering_logger = fp_EditorRenderingLogger;

        ////////////////////////////////////////////////
        // Generate Buffers
        ////////////////////////////////////////////////

        vector<float> vertices =
        {
            // Positions      // Texture Coords
            1.0f,  1.0f, 0.0f,   1.0f, 1.0f,  // Top Right
            1.0f, -1.0f, 0.0f,   1.0f, 0.0f,  // Bottom Right
           -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,  // Bottom Left
           -1.0f,  1.0f, 0.0f,   0.0f, 1.0f   // Top Left
        };

        vector<unsigned int> indices =
        {  // note that we start from 0!
            0, 1, 3,   // first triangle
            1, 2, 3    // second triangle
        };

        pm_VAO = pm_Render->Generate2DBuffers(vertices, indices);

        PeachCore::Print("The VAO ID for the Viewport Shader is: " + to_string(pm_VAO), PeachCore::Colours::Magenta);

        ////////////////////////////////////////////////
        // Shaders
        ////////////////////////////////////////////////

        string f_BaseDir = PHYSFS_getWriteDir(); //WARNING: USED ONLY FOR TESTING NEED THIS TO BE IN RESOURCELOADINGMANAGER

        pm_ViewportShader = new PeachCore::ShaderProgram
        (
            "Viewport Shader",
            f_BaseDir + "/shaders/viewport.vs",
            f_BaseDir + "/shaders/viewport.fs",
            editor_rendering_logger.get()
        );

        PeachCore::Print("The program ID for the Viewport Shader is: " + to_string(pm_ViewportShader->GetProgramID()), PeachCore::Colours::Magenta);

        ////////////////////////////////////////////////
        // Create Render Texture
        ////////////////////////////////////////////////

        if (not CreateRenderTexture(pm_CurrentViewportWidth, pm_CurrentViewportHeight))
        {
            PeachCore::PrintError("Was not able to create render texture");
        }
    }

    void
        Viewport::ResizeViewport
        (
            const unsigned int fp_Width,
            const unsigned int fp_Height
        )
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pm_FrameBuffer);

        ////////////////////////////////////////////////
        // Delete Old Render Texture
        ////////////////////////////////////////////////

        glDeleteTextures(1, &pm_RenderTexture);

        ////////////////////////////////////////////////
        // Generate New Render Texture
        ////////////////////////////////////////////////

        glGenTextures(1, &pm_RenderTexture);

        glBindTexture(GL_TEXTURE_2D, pm_RenderTexture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pm_CurrentViewportWidth, pm_CurrentViewportHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pm_RenderTexture, 0);

        ////////////////////////////////////////////////
        // Delete Old Render Buffer
        ////////////////////////////////////////////////
        glDeleteRenderbuffers(1, &pm_DepthRenderBuffer);

        ////////////////////////////////////////////////
        // Generate New Render Buffer
        ////////////////////////////////////////////////
        glGenRenderbuffers(1, &pm_DepthRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, pm_DepthRenderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, pm_CurrentViewportWidth, pm_CurrentViewportHeight);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, pm_DepthRenderBuffer);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void
        Viewport::RenderViewport
        (
            const glm::vec2& fp_Position,
            const unsigned int fp_Width,
            const unsigned int fp_Height
        )
    {
        if (pm_CurrentViewportWidth != fp_Width or pm_CurrentViewportHeight != fp_Height)
        {
            pm_CurrentViewportWidth = fp_Width;
            pm_CurrentViewportHeight = fp_Height;

            ResizeViewport(pm_CurrentViewportWidth, pm_CurrentViewportHeight);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, pm_FrameBuffer);

        glClearColor(1.0f, 0.3f, 0.3f, 1.0f);  // Red background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
        //glEnable(GL_DEPTH_TEST);

        if (not pm_Render)
        {
            throw runtime_error("no valid renderer for viewport");
        }

        //// Enable scissor test and set the scissor rectangle
        //glEnable(GL_SCISSOR_TEST);
        //glScissor(0, 0, fp_Width, fp_Height); // Set this to the area you want to clear

        glViewport(fp_Position.x, fp_Position.y, pm_CurrentViewportWidth, pm_CurrentViewportHeight);

        //pm_Render->DrawShapePrimitive(*pm_ViewportShader, pm_VAO);
        //glDisable(GL_SCISSOR_TEST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Bind to default framebuffer
        pm_Render->DrawTexture(*pm_ViewportShader, pm_VAO, pm_RenderTexture);
        //glDisable(GL_DEPTH_TEST);
    }


    bool
        Viewport::CreateRenderTexture
        (
            const unsigned int fp_Width,
            const unsigned int fp_Height
        )
    {
        pm_CurrentViewportWidth = fp_Width;
        pm_CurrentViewportHeight = fp_Height;

        ////////////////////////////////////////////////
        // Generate Frame Buffer
        ////////////////////////////////////////////////

        glGenFramebuffers(1, &pm_FrameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, pm_FrameBuffer);

        ////////////////////////////////////////////////
        // Generate Render Texture
        ////////////////////////////////////////////////

        glGenTextures(1, &pm_RenderTexture);

        glBindTexture(GL_TEXTURE_2D, pm_RenderTexture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pm_CurrentViewportWidth, pm_CurrentViewportHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pm_RenderTexture, 0);

        ////////////////////////////////////////////////
        // Generate Render Buffer
        ////////////////////////////////////////////////

        glGenRenderbuffers(1, &pm_DepthRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, pm_DepthRenderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, pm_CurrentViewportWidth, pm_CurrentViewportHeight);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, pm_DepthRenderBuffer);

        ////////////////////////////////////////////////
        // Setup Frame Buffer
        ////////////////////////////////////////////////

        // Check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            editor_rendering_logger->LogAndPrint("Error: Framebuffer is not complete!", "Viewport", PeachCore::LogManager::LogLevel::Error, "render_thread");
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            return false;
        }
       
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        ////////////////////////////////////////////////
        // Unbind Buffers and Reset GL state
        ////////////////////////////////////////////////

        editor_rendering_logger->LogAndPrint("Render Texture successfully setup UwU", "Viewport", PeachCore::LogManager::LogLevel::Debug, "render_thread");

        return true;
    }
}