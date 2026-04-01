/*******************************************************************
 *                        Peach-E v0.0.1
 *              Created by Ranyodh Mandur - 🍑 2024
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:         
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#include "GameManager.h"

#include <csignal>

namespace PeachCore {

    [[nodiscard]] static bool
        CreateSDLWindow
        (
            SDL_Window** fp_SDLWindow,
            const RendererType fp_RenderingBackend,
            const string& fp_WindowTitle,
            const unsigned int fp_WindowWidth,
            const unsigned int fp_WindowHeight,
            Logger* const logger
        )
    {
        if (*fp_SDLWindow)
        {
            logger->Error("Tried passing a valid SDL_Window* handle for window creation, please cleanup original SDL window or dereference pointer before attempting to create a new SDL window", "GameManager");
            return false;
        }

        uint64_t f_WindowFlags = 1;

        if (fp_RenderingBackend == RendererType::OpenGL)
        {
            f_WindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
        }
        else if (fp_RenderingBackend == RendererType::Vulkan)
        {
            f_WindowFlags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE;
        }
        else if (fp_RenderingBackend == RendererType::Metal)
        {
            f_WindowFlags = SDL_WINDOW_METAL | SDL_WINDOW_RESIZABLE;
        }
        else
        {
            logger->Error("Invalid Renderer Type was passed to CreateSDLWindow(), please pass a valid rendering backend type", "GameManager");
            return false;
        }

        *fp_SDLWindow = SDL_CreateWindow
        (
            fp_WindowTitle.c_str(),
            fp_WindowWidth,
            fp_WindowHeight,
            f_WindowFlags
        );

        if (not *fp_SDLWindow)
        {
            logger->Fatal("Window could not be created! SDL_Error: " + string(SDL_GetError()), "GameManager");
            return false;
        }

        SDL_WindowID f_WindowID = SDL_GetWindowID(*fp_SDLWindow);

        return true;
    }
}

namespace PeachCore
{
    //////////////////////////////////////////////
    // Initialization, Startup, and Shutdown/Cleanup OwO
    //////////////////////////////////////////////

    bool 
        GameManager::InitializePeachEngineCustom
        (
            const string& fp_RootPath,
            const uint8_t fp_RequiredThreads,
            const RendererType fp_RenderingBackend,
            bool fp_IsSegfaultHandled
        )
    {
        //////////////////// Use Peach-E default Segfault Handler ////////////////////

        if(not fp_IsSegfaultHandled)
        {
            signal(SIGSEGV, GameManager::SegFaultHandler); //XXX: used for trying to close and flush logs on seg fault
        }

        //////////////////// Required Threads Variable for Knowing Which Threads to Shutdown or Whatever ////////////////////

        pm_RequiredThreads = fp_RequiredThreads;

        //////////////////// Enable ANSI colour codes for windows console grumble grumble ////////////////////

        #if defined(PEACH_PLATFORM_WINDOWS) && defined(PEACH_USING_OS_TERMINAL)
            EnableWindowsConsoleColours();
        #endif

        ////////////////////////////////////////////// Initialize Main Thread Logger //////////////////////////////////////////////

        main_logger = Logger::CreateUnique("GameManager", PEACH_LOGGER_DEFAULT_FLAGS, fp_RootPath + "/logs");

        if (not main_logger)
        {
            PRINT_ERROR("[CRITICAL_LOGGING_ERROR]: GameManager failed to initialize the main_thread logger >w<");
            return false;
        }

        main_logger->Debug("main_thread logger successfully initialized", "GameManager");

        //////////////////// Initialize Subsystems ////////////////////

        if (not InitializePhysFS(fp_RootPath.c_str()))
        {
            main_logger->Fatal("Failed to initialize Peach Engine virtual file system, ending engine program execution immediately", "GameManager");
            return false;
        }
        else if (not SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) //YEAH THIS should be here oops idk how we created a SDL window before calling init oop
        {
            main_logger->Fatal(fmt::format("SDL could not initialize! ending engine program execution immediately, SDL_Error: {}", SDL_GetError()), "GameManager");
            return false;
        }
        else if (not CreateSDLWindow(&pm_MainWindow, fp_RenderingBackend, "Peach Window", 800, 600, main_logger.get()))
        {
            main_logger->Fatal("Initialization failed: Was not able to create the main window, exiting execution immediately", "GameManager");
            return false; //PEACH_ERROR_FAILED_TO_CREATE_MAIN_WINDOW;
        }
        else if (not InitializeThreads(fp_RootPath, fp_RenderingBackend))
        {
            main_logger->Fatal("Failed to initialize Peach Engine managers, ending engine program execution immediately", "GameManager");
            return false;
        }
        else if (not RetrieveQueues())
        {
            main_logger->Fatal("Command Queue acquisiton failed, exiting engine execution immediately", "GameManager");
            ShutdownPeachEngine();
            return false;
        }

        //LoadScriptRuntime(fp_BootConfPath, ScriptRuntimeType::Dotnet); //WARNING: this just loads the dotnet stuff for now

        //////////////////////////////////////////////
        // Load and Setup Plugins
        //////////////////////////////////////////////

        vector<string> f_ListOfPluginsToLoad;

        #ifdef PEACH_PLATFORM_WINDOWS //hard coded for now, will be dynamically loaded using a project file encoded in JSON or binary in the future
            //DLL's
            f_ListOfPluginsToLoad =
            {
                fp_RootPath + "/tests/plugins/SimplePlugin.dll",
                fp_RootPath + "/tests/plugins/SimplePlugin2.dll"
            };
        #else //Unix systems (osx and linux)
            //SO's or dylib
            f_ListOfPluginsToLoad =
            {
            };
        #endif

        //LoadPluginsFromConfigs(f_ListOfPluginsToLoad);

        InitializePlugins();

        //////////////////////////////////////////////
        // Load Startup Configs
        //////////////////////////////////////////////

        LoadGameStartupConfigs(); //used for telling peach engine which scene should be booted first, along with any other relevant startup routine instructions

        return true;
    }

    bool
        GameManager::InitializePeachEngine
        (
            const string& fp_RootPath
        )
    {
        //////////////////// Use Peach-E default Segfault Handler ////////////////////

        signal(SIGSEGV, GameManager::SegFaultHandler); //XXX: used for trying to close and flush logs on seg fault

        return true;
    }

    bool
        GameManager::ShutdownPeachEngine()  //CLEAN-UP AND ANY CLOSING THINGS THAT SHOULD BE LOGGED TO CHECK THE STATE OF THE ENGINE AS IT EXITS
    {
        //////////////////// close and cleanup everything script related ////////////////////

        //ShutdownPlugins();

        //////////////////// clean up in reverse order since a race condition can be created since every manager relies on resource manager's queues uwu ////////////////////
        
        if (pm_RequiredThreads & ThreadName::RenderThread)
        {
            RenderingManager::get_single().Stop();
        }
        if (pm_RequiredThreads & ThreadName::AudioThread)
        {
            AudioManager::get_single().Stop();
        }
        if (pm_RequiredThreads & ThreadName::PhysicsThread)
        {
            PhysicsManager::get_single().Stop();
        }
        if (pm_RequiredThreads & ThreadName::NetworkThread)
        {
            NetworkManager::get_single().Stop();
        }

        //don't need to check for usage here since ResourceManager is ALWAYS utilized regardless of what threads are desired uwu
        ResourceManager::get_single().Stop();

        return true;
    }

    bool
        GameManager::InitializePhysFS(const char* fp_RootPath)
    {
        if (not PHYSFS_init(fp_RootPath))
        {
            main_logger->Fatal("Failed to initialize PhysFS: " + static_cast<string>(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())), "GameManager");
            return false;
        }
        // Set the writable directory to the repo root
        else if (not PHYSFS_setWriteDir(fp_RootPath))
        {
            main_logger->Fatal("Failed to set write directory: " + static_cast<string>(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())), "GameManager");
            return false;
        }
        // Mount the root directory for asset loading
        else if (not PHYSFS_mount(fp_RootPath, nullptr, 1))
        {
            main_logger->Fatal("Failed to set search path: " + static_cast<string>(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())), "GameManager");
            return false;
        }

        main_logger->Debug("PhysFS initialized at root: " + static_cast<string>(fp_RootPath), "GameManager");
        return true;
    }

    //TODO: get a signal from each thread when initialization is done then start the next one so startup can be consisitent and no race conditions for safety
    bool
        GameManager::InitializeThreads //XXX: used for kickstarting threads needed for engine execution
        (
            const string& fp_RootPath,
            RendererType fp_RenderingBackend,
            const bool fp_Is3D
        )
    {
        const string f_LogDir = fp_RootPath + "/logs";

        ////////////////////////////////////////////// Resource Loading //////////////////////////////////////////////

        //IMPORTANT: resource thread needs to be initialized first so queues get created properly
        //Lazy initialization is used for everything since it isnt guaranteed that all threads will be active, only if a node is needed for a corresponding thread then the thread is started
        //otherwise we just leave it be

        //always going to require resource thread for loading peachey
        pm_ResourceThread = jthread
        (
            &ResourceManager::ResourceLoop, 
            std::ref(ResourceManager::get_single()), 
            f_LogDir, 
            fp_RootPath, 
            std::ref(pm_ResourceInitializationLatch)
        );

        pm_ResourceInitializationLatch.wait(); //wait for resource thread to initialize before going further w any other threads uwu

        ////////////////////////////////////////////// Rendering //////////////////////////////////////////////

        if (pm_RequiredThreads & ThreadName::RenderThread)
        {
#ifdef PEACH_RENDERER_VULKAN
            if(fp_RenderingBackend == RendererType::Vulkan)
            {
                pm_RenderThread = jthread
                (
                    &RenderingManager::RenderLoopVK,
                    std::ref(RenderingManager::get_single()),
                    f_LogDir,
                    std::ref(pm_ThreadInitializationLatch),
                    pm_MainWindow,
                    10
                );
            }
#endif

#ifdef PEACH_RENDERER_OPENGL
            if (fp_RenderingBackend == RendererType::OpenGL)
            {
                pm_RenderThread = jthread
                (
                    &RenderingManager::RenderLoopGL,
                    std::ref(RenderingManager::get_single()),
                    f_LogDir,
                    std::ref(pm_ThreadInitializationLatch),
                    pm_MainWindow,
                    10
                );
            }
#endif

#ifdef PEACH_RENDERER_METAL
            if (fp_RenderingBackend == RendererType::Metal)
            {
                pm_RenderThread = jthread
                (
                    &RenderingManager::RenderLoopMetal,
                    std::ref(RenderingManager::get_single()),
                    f_LogDir,
                    std::ref(pm_ThreadInitializationLatch),
                    pm_MainWindow,
                    10
                );
            }
#endif
        }
        else
        {
            pm_ThreadInitializationLatch.count_down();
        }

        ////////////////////////////////////////////// Audio //////////////////////////////////////////////

        if (pm_RequiredThreads & ThreadName::AudioThread)
        {
            pm_AudioThread = jthread
            (
                &AudioManager::AudioLoop, 
                std::ref(AudioManager::get_single()), 
                f_LogDir, 
                0.0f, 
                std::ref(pm_ThreadInitializationLatch)
            );
        }
        else
        {
            pm_ThreadInitializationLatch.count_down();
        }

        ////////////////////////////////////////////// Networking //////////////////////////////////////////////

        if (pm_RequiredThreads & ThreadName::NetworkThread)
        {
            pm_NetworkThread = jthread
            (
                &NetworkManager::NetworkLoop,
                std::ref(NetworkManager::get_single()), 
                f_LogDir, 
                std::ref(pm_ThreadInitializationLatch)
            );
        }
        else
        {
            pm_ThreadInitializationLatch.count_down();
        }

        ////////////////////////////////////////////// Physics //////////////////////////////////////////////

        if (pm_RequiredThreads & ThreadName::PhysicsThread)
        {
            if(fp_Is3D)
            {
                pm_PhysicsThread = jthread
                (
                    &PhysicsManager::PhysicsLoop3D, 
                    std::ref(PhysicsManager::get_single()),
                    f_LogDir, 
                    std::ref(pm_ThreadInitializationLatch)
                );
            }
            else
            {
                pm_PhysicsThread = jthread
                (
                    &PhysicsManager::PhysicsLoop2D, 
                    std::ref(PhysicsManager::get_single()), 
                    f_LogDir, 
                    std::ref(pm_ThreadInitializationLatch),
                    0.0f,    // fp_GravityX
                    -9.8f    // fp_GravityY
                );
            }
        }
        else
        {
            pm_ThreadInitializationLatch.count_down();
        }

        PRINT("Hello World!\n", Colours::Blue); //>w<
        main_logger->Warning("NEW ENGINE ON THE BLOCK MY SLIME", "Peach-E");
        main_logger->Trace("Success! This Built Correctly", "Peach-E");

        pm_ThreadInitializationLatch.wait();

        return true;
    }

    bool
        GameManager::RetrieveQueues()
    {
        //////////////////// Get Resource Loading Command Queue ////////////////////

        pm_ResourceCommandQueue = ResourceManager::get_single().GetLoadCommandQueue(main_logger.get());

        if (not pm_ResourceCommandQueue)
        {
            main_logger->Fatal("Failed to retrieve Resource Loading Command Queue from ResourceManager, engine cannot continue execution", "GameManager");
            return false;
        }

        main_logger->Info("Successfully retrieved Resource Loading Command Queue from ResourceManager", "GameManager");

        //////////////////// Get Draw Command Queue ////////////////////

        if (pm_RequiredThreads & ThreadName::RenderThread)
        {
            pm_RenderCommandQueue = RenderingManager::get_single().GetDrawCommandQueue(main_logger.get());

            if (not pm_RenderCommandQueue)
            {
                main_logger->Fatal("Failed to retrieve Draw Command Queue from RenderingManager, engine cannot continue execution", "GameManager");
                return false;
            }

            main_logger->Info("Successfully retrieved Draw Command Queue from RenderingManager", "GameManager");
        }

        //////////////////// Get Audio Command Queue ////////////////////

        if (pm_RequiredThreads & ThreadName::AudioThread)
        {
            pm_AudioCommandQueue = AudioManager::get_single().GetAudioCommandQueue(main_logger.get());

            if (not pm_AudioCommandQueue)
            {
                main_logger->Fatal("Failed to retrieve Audio Command Queue from AudioManager, engine cannot continue execution", "GameManager");
                return false;
            }

            main_logger->Info("Successfully retrieved Audio Command Queue from AudioManaager", "GameManager");
        }

        //////////////////// Get Resource Loading Command Queue ////////////////////

        if (pm_RequiredThreads & ThreadName::NetworkThread)
        {
            pm_NetworkCommandQueue = NetworkManager::get_single().GetNetworkCommandQueue(main_logger.get());

            if (not pm_NetworkCommandQueue)
            {
                main_logger->Fatal("Failed to retrieve Network Command Queue from ResourceManager, engine cannot continue execution", "GameManager");
                return false;
            }

            main_logger->Info("Successfully retrieved Resource Loading Command Queue from ResourceManager", "GameManager");
        }

        //////////////////// Get Resource Loading Command Queue ////////////////////

        if (pm_RequiredThreads & ThreadName::PhysicsThread)
        {
            pm_PhysicsCommandQueue = PhysicsManager::get_single().GetPhysicsCommandQueue(main_logger.get());

            if (not pm_PhysicsCommandQueue)
            {
                main_logger->Fatal("Failed to retrieve Resource Loading Command Queue from ResourceManager, engine cannot continue execution", "GameManager");
                return false;
            }

            main_logger->Info("Successfully retrieved Resource Loading Command Queue from ResourceManager", "GameManager");
        }

        return true;
    }

    bool
        GameManager::SetupScriptRuntime
        (
            const string& fp_BootConfPath,
            const uint8_t fp_RequiredScriptRuntimes
        )
    {
        //if (fp_RequiredScriptRuntimes & ScriptRuntimeType::Dotnet and not ResourceManager::get_single().LoadDotNetRuntime(fp_BootConfPath, pm_DotnetContext))
        //{

        //    return false;
        //}
        if (fp_RequiredScriptRuntimes & static_cast<uint8_t>(ScriptRuntimeType::Lua))
        {

            return false;
        }

        if (fp_RequiredScriptRuntimes & static_cast<uint8_t>(ScriptRuntimeType::BongoJam))
        {

            return false;
        }

            return true;
    }

    //////////////////////////////////////////////
    // Main Loop for Peach Engine >O<
    //////////////////////////////////////////////

    void
        GameManager::StartMainGameLoop()
    {
        const float PHYSICS_TIME_STEP = 1.0f / USER_DEFINED_CONSTANT_UPDATE_FPS;  // Fixed physics update rate 
        float INPUT_POLL_TIME_STEP = 1.0f / USER_DEFINED_RENDER_FPS;  // Should be variable to allow dynamic adjustment in-game

        float f_PhysicsAccumulator = 0.0f;
        float f_InputAccumulator = 0.0f;

        auto f_CurrentTime = chrono::high_resolution_clock::now();

        auto rendering_manager = &RenderingManager::get_single();
        auto network_manager = &NetworkManager::get_single();
        auto physics_manager = &PhysicsManager::get_single();
        auto resource_manager = &ResourceManager::get_single();

        ///TODO: log whenever frametime is running late in debug
        while (m_IsRunning.load(std::memory_order_acquire))
        {
            auto f_NewTime = chrono::high_resolution_clock::now();
            float f_FrameTime = chrono::duration<float>(f_NewTime - f_CurrentTime).count();
            f_CurrentTime = f_NewTime;

            //////////////////// Prevent spiral of death by clamping frame time, frames will be skipped, but if you're already this behind then thats the least of your problems lmao ////////////////////

            if (f_FrameTime > 0.25)
            {
                f_FrameTime = 0.25;
            }

            //////////////////// Increment Accumulators ////////////////////

            f_PhysicsAccumulator += f_FrameTime;
            f_InputAccumulator += f_FrameTime;

            //////////////////// Poll Inputs OwO ////////////////////

            if (f_InputAccumulator >= INPUT_POLL_TIME_STEP)
            {
                PollUserInputEvents();
                f_InputAccumulator -= INPUT_POLL_TIME_STEP;
            }

            //////////////////// Physics and fixed interval updates ////////////////////

            if (f_PhysicsAccumulator >= PHYSICS_TIME_STEP)
            {
                size_t f_Steps = 0;

                while (f_PhysicsAccumulator >= PHYSICS_TIME_STEP)
                {
                    f_PhysicsAccumulator -= PHYSICS_TIME_STEP;
                    f_Steps++;
                }

                float f_ScaledDt = PHYSICS_TIME_STEP * pm_CurrentTimeScale;

                physics_manager->RequestPhysicsWorldStep(f_ScaledDt, f_Steps);
                CallConstantUpdate(f_ScaledDt);

                pm_CurrentScene.CleanSceneTree(); //Check for any node removals uwu, done everytime after scripts are ran to check for queued for removal nodes uwu
            }
        }
    }

    //////////////////////////////////////////////
    // Plugin Stuff
    //////////////////////////////////////////////

    void
        GameManager::LoadPluginsFromConfigs(const vector<string>& fp_ListOfPluginsToLoad)
    {
        for (const auto& lv_PluginPath : fp_ListOfPluginsToLoad)
        {
            NativeScriptData f_TempPlugin = {};
            ResourceManager::get_single().LoadNativeSciptInstanceFFS(lv_PluginPath, f_TempPlugin);

            pm_NativeScriptPlugins.emplace_back(move(f_TempPlugin.Instance), f_TempPlugin.Handle);
        }
    }

    void 
        GameManager::InitializePlugins()
        const
    {
        for (auto& lv_PluginInfo : pm_NativeScriptPlugins)
        {
            lv_PluginInfo.Instance->Initialize();
        }
    }

    void 
        GameManager::UpdatePlugins(float fp_TimeSinceLastFrame)
        const
    {
        for (auto& lv_PluginInfo : pm_NativeScriptPlugins)
        {
            lv_PluginInfo.Instance->Update(fp_TimeSinceLastFrame);
        }
    }

    void 
        GameManager::ConstantUpdatePlugins(float fp_TimeSinceLastFrame)
        const
    {
        for (auto& lv_PluginInfo : pm_NativeScriptPlugins)
        {
            lv_PluginInfo.Instance->ConstantUpdate(fp_TimeSinceLastFrame);
        }
    }

    void 
        GameManager::ShutdownPlugins()
    {
        for (auto& lv_PluginInfo : pm_NativeScriptPlugins)
        {
            lv_PluginInfo.Instance->Shutdown(); //plugin devs better cleanup after themselves, nothing I can do to ensure safety here uwu

            if (lv_PluginInfo.Handle != nullptr)
            {
                DYNLIB_UNLOAD(lv_PluginInfo.Handle);
            }

            lv_PluginInfo.Instance.reset(); //clear plugin and let it delete but should change this to be explicit and not inside the plugin itself shutdown is sufficient tbh
        }

        pm_NativeScriptPlugins.clear(); //wait why am i clearing plugin handles before unloading them LMFAO, XXX: fixed it uwu ><
    }

    //////////////////////////////////////////////
    // Core Runner Functions
    //////////////////////////////////////////////

    void
        GameManager::CallUpdate(double fp_MilisecondsSinceLastCall)
    {
        //process shit by calling the python/lua/dotnet runtime on the Update() functions defined inside the scripts
    }

    void
        GameManager::CallConstantUpdate(double fp_FixedDeltaTime)
    {
        //process shit by calling the python/lua/dotnet runtime on the Update()/ConstantUpdate() functions defined inside the scripts
    }

    //////////////////////////////////////////////
    // Window Stuff
    //////////////////////////////////////////////

    void
        GameManager::PollUserInputEvents()
    {
        InputManager::get_single().PollEvents();

        InputManager::get_single().GetWindowCloseRequests(pm_CloseWindowRequests);

        for (const auto& lv_Window : pm_CloseWindowRequests)
        {
            if (SDL_GetWindowID(pm_MainWindow) == lv_Window)
            {
                m_IsRunning.store(false, std::memory_order_release);
            }

            SDL_DestroyWindow(SDL_GetWindowFromID(lv_Window)); //WARNING DO NOT CLOSE WINDOW HERE SEND A REQUEST TO THE RENDERING MANAGER FOR THAT
        }

        glm::vec2 f_MousePos = InputManager::get_single().GetCurrentMousePosition();

        PRINT(fmt::format("mouse x : {}, y: {}", f_MousePos.x, f_MousePos.y), Colours::Green);
    }
    //////////////////////////////////////////////
    // Peach API Functions
    //////////////////////////////////////////////

    [[nodiscard]] PEACH_STATUS_CODE
        GameManager::ChangeScene(const string& fp_DesiredSceneName)
    {
        if (pm_DictionaryOfAllScenesInCurrentProject.find(fp_DesiredSceneName) == pm_DictionaryOfAllScenesInCurrentProject.end())
        {
            return PEACH_ERROR_INVALID_SCENE_NAME;
        }

        //LoadScene(fp_DesiredSceneName);

        return PEACH_OK;
    }

    [[nodiscard]] SceneTree*
        GameManager::GetCurrentScene()
    {
        return &pm_CurrentScene;
    }
}