/*******************************************************************
 *                        Peach-E v0.0.1
 *         Created by Ranyodh Singh Mandur - 🍑 2024-2026
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#include "GameManager.h"
#include "InputManager.h"
#include "debug/CrashSignalHandler.h"
#include "utils/PeachPrint.h"

#include <SDL3/SDL_main.h>

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
        if(not logger) [[unlikely]]
        {
            PEACH_PRINT_ERROR("tried to pass nullptr reference to logger inside GameManager::CreateSDLWindow()");
            return false;
        }

        if (*fp_SDLWindow) [[unlikely]] //otherwise can't guarantee the original window was cleaned up and that's a problem ;w;
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
        else if (fp_RenderingBackend == RendererType::WebGL or fp_RenderingBackend == RendererType::MobileGL)
        {
            //WebGL and OpenGL ES both use SDL_WINDOW_OPENGL.
            //GLES context attributes MUST be set before SDL_CreateWindow, not after.
            //On WASM, SDL3 + Emscripten translate these to a WebGL2 context automatically.
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

            f_WindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
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
            logger->Fatal(fmt::format("Window could not be created! SDL_Error: {}", SDL_GetError()), "GameManager");
            return false;
        }

        return true;
    }
}

namespace PeachCore
{
    /*
        dont needa check required threads here since joinable is false on default constructed threads
        and if the thread was never created, then it'd just stay in that default constructed state owo
    */

    //////////////////////////////////////////////
    // Initialization, Startup, and Shutdown/Cleanup OwO
    //////////////////////////////////////////////

    bool 
        GameManager::InitializePeachEngineCustom
        (
            const string& fp_RootPath,
            const RequiredSubsystems fp_RequiredSubsystems,
            const RendererType fp_RenderingBackend,
            const uint32_t fp_StartingWindowWidth,
            const uint32_t fp_StartingWindowHeight,
            bool fp_IsSegfaultHandled
        )
    {
        //////////////////// Use Peach-E default Segfault Handler ////////////////////

        if(not fp_IsSegfaultHandled)
        {
            Debug::InstallCrashHandler(); //XXX: used for trying to close and flush logs on seg fault
        }

        //////////////////// SDL is fucking weird mang ////////////////////

        #ifdef SDL_MAIN_HANDLED 
            SDL_SetMainReady(); // Required when SDL_MAIN_HANDLED is defined
        #endif

        //////////////////// Required Threads Variable for Knowing Which Threads to Shutdown or Whatever ////////////////////

        pm_ActiveSubsystems = fp_RequiredSubsystems;

        //////////////////// Enable ANSI colour codes for windows console grumble grumble ////////////////////

        #if defined(PEACH_PLATFORM_WINDOWS) && defined(PEACH_USING_OS_TERMINAL)
            if (not PEACH_EnableWindowsConsoleColours())
            {
                PEACH_PRINT_ERROR("Unable to set console mode, and enable ANSI colour codes on windows terminal owo");
            }
        #endif

        ////////////////////////////////////////////// Initialize Main Thread Logger //////////////////////////////////////////////

        main_logger = LogManager::get_single().CreateUniqueLogger("GameManager", PEACH_LOGGER_DEFAULT_FLAGS, fp_RootPath + "/logs");

        if (not main_logger)
        {
            PEACH_PRINT_ERROR("[CRITICAL_LOGGING_ERROR]: GameManager failed to initialize the main_thread logger >w<");
            return false;
        }

        main_logger->Info("main_thread logger successfully initialized", "GameManager");

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
        else if (not CreateSDLWindow(&pm_MainWindow, fp_RenderingBackend, "Peach Window", fp_StartingWindowWidth, fp_StartingWindowHeight, main_logger.get()))
        {
            main_logger->Fatal("Initialization failed: Was not able to create the main window, exiting execution immediately", "GameManager");
            return false; //PEACH_ERROR_FAILED_TO_CREATE_MAIN_WINDOW;
        }
        else if (not InputManager::get_single().Initialize(fp_StartingWindowWidth, fp_StartingWindowHeight, fp_RootPath + "/logs", PEACH_LOGGER_DEFAULT_FLAGS)) //IMPORTANT: NEEDS TO INITIALIZE BEFORE RENDERING MANAGER
        {

            return false;
        }
        else if (not InitializeThreads(fp_RootPath, fp_StartingWindowWidth, fp_StartingWindowHeight, fp_RenderingBackend))
        {
            main_logger->Fatal("Failed to initialize Peach Engine managers, ending engine program execution immediately", "GameManager");
            
            //XXX: idk we needa do smth if only some threads initialize really owo so the engine doesnt just hang >w<
            ShutdownPeachEngine(); //shutdown threads now that their initialized owo
            
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

        //InitializePlugins();

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

        PEACH_TO_DO_UNUSED(fp_RootPath);

        return true;
    }

    bool
        GameManager::ShutdownPeachEngine()  //CLEAN-UP AND ANY CLOSING THINGS THAT SHOULD BE LOGGED TO CHECK THE STATE OF THE ENGINE AS IT EXITS
    {
        //////////////////// close and cleanup everything script related ////////////////////

        //ShutdownPlugins();

        //////////////////// clean up in reverse order since a race condition can be created since every manager relies on resource manager's queues uwu ////////////////////
        
        if (pm_ActiveSubsystems & Subsystem::Render)
        {
            pm_RenderingManager.ShutdownSubsystem(main_logger.get());
        }
        if (pm_ActiveSubsystems & Subsystem::Audio)
        {
            pm_AudioManager.ShutdownSubsystem(main_logger.get());
        }
        if (pm_ActiveSubsystems & Subsystem::Physics2D or pm_ActiveSubsystems & Subsystem::Physics3D)
        {
            pm_PhysicsManager.ShutdownSubsystem(main_logger.get());
        }
        if (pm_ActiveSubsystems & Subsystem::Network)
        {
            pm_NetworkManager.ShutdownSubsystem(main_logger.get());
        }

        //don't need to check for usage here since ResourceManager is ALWAYS utilized regardless of what threads are desired uwu
        pm_ResourceManager.ShutdownSubsystem(main_logger.get());

        // SDL_Quit();

        return true;
    }

    bool
        GameManager::InitializePhysFS(const char* fp_RootPath)
    {
        /*
            PHYSFS_init(argv0) uses argv0 as a hint to locate the base directory.
            On Android, /proc/self/exe points to app_process64 (ART launcher), NOT
            our .so — physfs corrupts its platform function table on the resulting
            path mangling, branching to ASCII bytes as a function pointer → SIGSEGV.

            Passing nullptr skips the argv0-based base dir calculation and uses
            platform defaults instead. We then mount fp_RootPath manually.

            On all other platforms, passing fp_RootPath is fine because it maps
            to argv[0] which physfs uses to find the game executable directory.
        */

        #ifdef PEACH_PLATFORM_ANDROID
            const char* f_PhysFSArgv0 = nullptr;
        #else
            const char* f_PhysFSArgv0 = fp_RootPath;
        #endif

        if (not PHYSFS_init(f_PhysFSArgv0))
        {
            main_logger->Fatal(fmt::format("Failed to initialize PhysFS: {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())), "GameManager");
            return false;
        }
        // Set the writable directory to the repo root
        else if (not PHYSFS_setWriteDir(fp_RootPath))
        {
            main_logger->Fatal(fmt::format("Failed to set write directory: {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())), "GameManager");
            return false;
        }
        // Mount the root directory for asset loading
        else if (not PHYSFS_mount(fp_RootPath, nullptr, 1))
        {
            main_logger->Fatal(fmt::format("Failed to set search path: {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())), "GameManager");
            return false;
        }

        main_logger->Info(fmt::format("PhysFS initialized at root: {}", fp_RootPath), "GameManager");
        return true;
    }

    //TODO: get a signal from each thread when initialization is done then start the next one so startup can be consisitent and no race conditions for safety
    PEACH_STATUS_CODE
        GameManager::InitializeThreads //XXX: used for kickstarting threads needed for engine execution
        (
            const string& fp_RootPath,
            const uint32_t fp_InitialWindowWidth,
            const uint32_t fp_InitialWindowHeight,
            RendererType fp_RenderingBackend,
            const bool fp_IsRendering3D
        )
    {
        PEACH_TO_DO_UNUSED(fp_InitialWindowWidth);
        PEACH_TO_DO_UNUSED(fp_InitialWindowHeight);
        PEACH_TO_DO_UNUSED(fp_IsRendering3D);

        const string f_LogDir = fp_RootPath + "/logs";

        ////////////////////////////////////////////// Resource Loading //////////////////////////////////////////////

        //IMPORTANT: resource thread needs to be initialized first so queues get created properly
        //Lazy initialization is used for everything since it isnt guaranteed that all threads will be active, only if a node is needed for a corresponding thread then the thread is started
        //otherwise we just leave it be

        //always going to require resource thread for loading peachey

        if (not pm_ResourceManager.Initialize(f_LogDir, fp_RootPath))
        {

            return PEACH_FATAL_ERROR_FAILED_TO_INITIALIZE_RESOURCE_MANAGER;
        }


        ////////////////////////////////////////////// Rendering //////////////////////////////////////////////

        if (pm_ActiveSubsystems & Subsystem::Render)
        {
            PEACH_STATUS_CODE result = pm_RenderingManager.Initialize(fp_RenderingBackend, pm_ResourceManager.GetDrawableResourceLoadingQueue(main_logger.get()), pm_MainWindow, fp_InitialWindowWidth, fp_InitialWindowHeight, 10, f_LogDir);

            if(result != PEACH_OK)
            {
                return result;
            }
        }

        ////////////////////////////////////////////// Audio //////////////////////////////////////////////

        if (pm_ActiveSubsystems & Subsystem::Audio)
        {
            if (not pm_AudioManager.InitializeAudioEngine(100.0f, pm_ResourceManager.GetAudioResourceLoadingQueue(main_logger.get()), f_LogDir))
            {

                return PEACH_FATAL_ERROR_FAILED_TO_INITIALIZE_AUDIO_SUBSYSTEM;
            }

        }

        ////////////////////////////////////////////// Networking //////////////////////////////////////////////

        if (pm_ActiveSubsystems & Subsystem::Network)
        {
            if(not pm_NetworkManager.InitializeNetworking(f_LogDir))
            {

                return PEACH_FATAL_ERROR_FAILED_TO_INITIALIZE_NETWORKING_UWU;
            }
        }

        ////////////////////////////////////////////// Physics //////////////////////////////////////////////

        if (pm_ActiveSubsystems & Subsystem::Physics3D)
        {            
            if(not pm_PhysicsManager.InitializePhysicsEngine3D(f_LogDir))
            {

                return PEACH_FATAL_ERROR_FAILED_TO_INITIALIZE_PHYSICS_3D;
            }
        }
        else if (pm_ActiveSubsystems & Subsystem::Physics2D)
        {
            if(not pm_PhysicsManager.InitializePhysicsEngine2D(f_LogDir, 0.0f, 0.0f))
            {

                return PEACH_FATAL_ERROR_FAILED_TO_INITIALIZE_PHYSICS_2D;
            }
        }

        PEACH_PRINT("Hello World!\n", PEACH_COL_BLUE); //>w<
        main_logger->Warning("NEW ENGINE ON THE BLOCK MY SLIME", "Peach-E");
        PEACH_LOG_TRACE(main_logger, "Success! This Built Correctly", "Peach-E");

        return PEACH_OK;
    }

    bool
        GameManager::SetupScriptRuntime
        (
            const string& fp_BootConfPath,
            const uint8_t fp_RequiredScriptRuntimes
        )
    {
        PEACH_TO_DO_UNUSED(fp_BootConfPath); //ACTUALLY ASSIGN THIS OWO



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

    #ifdef PEACH_PLATFORM_WASM

        #include <emscripten/emscripten.h>
        #include <emscripten/html5.h>

        namespace
        {
            //emscripten requires a C-style callback. We stash the GameManager singleton ptr
            //in a file-scope variable since the loop has no userdata channel that's worth
            //the boilerplate of wrapping our own. GameManager is already a singleton anyway.
            GameManager* g_WasmLoopOwner = nullptr;

            //per-frame timing state — needs to persist across callback invocations
            chrono::high_resolution_clock::time_point g_LastFrameTime;
            float g_PhysicsAccumulator = 0.0f;
            bool g_LoopInitialized = false;

            void 
                PeachWasmDispatch() //em_callback_func compatible: void(*)()
            {
                if (g_WasmLoopOwner)
                {
                    g_WasmLoopOwner->___________________WasmFrameCallback();
                }
            }
        }

        void
            GameManager::___________________WasmFrameCallback()
        {
            if (not g_WasmLoopOwner)
            {
                return; //loop owner gone, can happen during shutdown
            }

            if (not g_WasmLoopOwner->m_IsRunning.load(std::memory_order_acquire))
            {
                //engine signaled shutdown — stop the rAF loop
                emscripten_cancel_main_loop();
                return;
            }

            ////////////////// Per-frame timing init on first call //////////////////

            if (not g_LoopInitialized)
            {
                g_LastFrameTime = chrono::high_resolution_clock::now();
                g_LoopInitialized = true;
            }

            const float f_PhysicsTimeStep = 1.0f / USER_DEFINED_CONSTANT_UPDATE_FPS;

            ////////////////// Frame timing //////////////////

            auto f_NewTime = chrono::high_resolution_clock::now();
            float f_FrameTime = chrono::duration<float>(f_NewTime - g_LastFrameTime).count();
            g_LastFrameTime = f_NewTime;

            if (f_FrameTime > 0.25f) //spiral-of-death clamp
            {
                f_FrameTime = 0.25f;
            }

            g_PhysicsAccumulator += f_FrameTime;

            ////////////////// Inputs //////////////////
            //input polling is locked to render rate on WASM since SDL_PollEvent must run on
            //the main thread which is also the rAF callback thread. We can't have a separate
            //polling rate like desktop does — browser doesn't allow it.

            g_WasmLoopOwner->PollUserInputEvents();

            ////////////////// Physics and fixed-step updates //////////////////

            if (g_PhysicsAccumulator >= f_PhysicsTimeStep)
            {
                size_t f_Steps = 0;

                while (g_PhysicsAccumulator >= f_PhysicsTimeStep)
                {
                    g_PhysicsAccumulator -= f_PhysicsTimeStep;
                    f_Steps++;
                }

                float f_ScaledDt = f_PhysicsTimeStep * g_WasmLoopOwner->pm_CurrentTimeScale;

                PhysicsManager::get_single().RequestPhysicsWorldStep(f_ScaledDt, f_Steps);
                g_WasmLoopOwner->CallConstantUpdate(f_ScaledDt);
                g_WasmLoopOwner->pm_CurrentScene.CleanSceneTree();
            }

            ////////////////// Render //////////////////
            //rendering must happen on the main thread for WebGL. RenderingManager's
            //thread-based RenderLoopVK/RenderLoopGL pattern doesn't work here — we need
            //a single-frame render call that runs synchronously in the callback.
            //
            //TODO: implement RenderingManager::RenderSingleFrame() that does what one
            //iteration of the desktop render loop body does, then call it here.

            // RenderingManager::get_single().RenderSingleFrame();

            //returning from this function yields control back to the browser, which
            //schedules the next rAF callback in ~16ms (60Hz) or ~8ms (120Hz). Browser
            //will throttle this to 1Hz when the tab is backgrounded.
        }

        void
            GameManager::StartMainGameLoop()
        {
            g_WasmLoopOwner = this;
            g_LoopInitialized = false;

            //args:
            //  fp_FuncPtr — the per-frame callback
            //  fp_Fps     — 0 means "use requestAnimationFrame's natural rate" (recommended)
            //  fp_SimulateInfiniteLoop — must be 0, otherwise emscripten transforms the
            //                            program into "throw to escape main(), restart on
            //                            next frame" which breaks RAII teardown completely.
            //                            With 0, this returns immediately, the rAF loop
            //                            runs in the background, and main() returns
            //                            normally — but DON'T let main() actually exit
            //                            because that destroys all globals. The Emscripten
            //                            runtime keeps your wasm module alive after main()
            //                            returns as long as you don't call exit().

            emscripten_set_main_loop(PeachWasmDispatch, 0, 0); // free fn, not member fn
            //control returns here immediately. main() in WasmMain.cpp must return without
            //calling ShutdownPeachEngine — engine teardown happens via an exit handler or
            //user navigation away from the page.
        }

    #else

        void
            GameManager::StartMainGameLoop()
        {
            const float PHYSICS_TIME_STEP = 1.0f / USER_DEFINED_CONSTANT_UPDATE_FPS;  // Fixed physics update rate 
            float INPUT_POLL_TIME_STEP = 1.0f / USER_DEFINED_POLLING_RATE;  // Should be variable to allow dynamic adjustment in-game

            float f_PhysicsAccumulator = 0.0f;
            float f_InputAccumulator = 0.0f;

            auto f_CurrentTime = chrono::high_resolution_clock::now();

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

                    pm_PhysicsManager.RequestPhysicsWorldStep(f_ScaledDt, f_Steps);
                    CallConstantUpdate(f_ScaledDt);

                    pm_CurrentScene.CleanSceneTree(); //Check for any node removals uwu, done everytime after scripts are ran to check for queued for removal nodes uwu
                }
            }
        }

    #endif /*PEACH_PLATFORM_WASM*/

    //////////////////////////////////////////////
    // Plugin Stuff
    //////////////////////////////////////////////

    //void
    //    GameManager::LoadPluginsFromConfigs(const vector<string>& fp_ListOfPluginsToLoad)
    //{
    //    for (const auto& lv_PluginPath : fp_ListOfPluginsToLoad)
    //    {
    //        NativeScriptData f_TempPlugin = {};
    //        ResourceManager::get_single().LoadNativeSciptInstanceFFS(lv_PluginPath, f_TempPlugin);

    //        pm_NativeScriptPlugins.emplace_back(move(f_TempPlugin.Instance), f_TempPlugin.Handle);
    //    }
    //}

    //void 
    //    GameManager::InitializePlugins()
    //    const
    //{
    //    for (auto& lv_PluginInfo : pm_NativeScriptPlugins)
    //    {
    //        lv_PluginInfo.Instance->Initialize();
    //    }
    //}

    //void 
    //    GameManager::UpdatePlugins(float fp_TimeSinceLastFrame)
    //    const
    //{
    //    for (auto& lv_PluginInfo : pm_NativeScriptPlugins)
    //    {
    //        lv_PluginInfo.Instance->Update(fp_TimeSinceLastFrame);
    //    }
    //}

    //void 
    //    GameManager::ConstantUpdatePlugins(float fp_TimeSinceLastFrame)
    //    const
    //{
    //    for (auto& lv_PluginInfo : pm_NativeScriptPlugins)
    //    {
    //        lv_PluginInfo.Instance->ConstantUpdate(fp_TimeSinceLastFrame);
    //    }
    //}

    //void 
    //    GameManager::ShutdownPlugins()
    //{
    //    for (auto& lv_PluginInfo : pm_NativeScriptPlugins)
    //    {
    //        lv_PluginInfo.Instance->Shutdown(); //plugin devs better cleanup after themselves, nothing I can do to ensure safety here uwu

    //        if (lv_PluginInfo.Handle != nullptr)
    //        {
    //            DYNLIB_UNLOAD(lv_PluginInfo.Handle);
    //        }

    //        lv_PluginInfo.Instance.reset(); //clear plugin and let it delete but should change this to be explicit and not inside the plugin itself shutdown is sufficient tbh
    //    }

    //    pm_NativeScriptPlugins.clear(); //wait why am i clearing plugin handles before unloading them LMFAO, XXX: fixed it uwu ><
    //}

    //////////////////////////////////////////////
    // Core Runner Functions
    //////////////////////////////////////////////

    void
        GameManager::CallUpdate(double fp_MilisecondsSinceLastCall)
    {
        //process shit by calling the python/lua/dotnet runtime on the Update() functions defined inside the scripts
        PEACH_TO_DO_UNUSED(fp_MilisecondsSinceLastCall);
    }

    void
        GameManager::CallConstantUpdate(double fp_FixedDeltaTime)
    {
        //process shit by calling the python/lua/dotnet runtime on the Update()/ConstantUpdate() functions defined inside the script
        PEACH_TO_DO_UNUSED(fp_FixedDeltaTime);
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
            if (SDL_GetWindowID(pm_MainWindow) == lv_Window) //ERROR: THIS WILL NOT CLOSE GRACEFULLY but w/e the driver will pick it up :^)
            {
                m_IsRunning.store(false, std::memory_order_release);
                // RenderingManager::get_single().Stop(); //stop rendering but also should wait probably here since windows closing
                // if(pm_RenderThread.joinable())
                // {
                //     pm_RenderThread.join();
                //     PEACH_PRINT("Successfully joined render thread on close owo", PEACH_COL_BRIGHT_GREEN);
                // }
                // else {
                //     PEACH_PRINT_ERROR("Failed to join render thread on close ;w;");
                // }
            }

            SDL_DestroyWindow(SDL_GetWindowFromID(lv_Window)); //WARNING DO NOT CLOSE WINDOW HERE SEND A REQUEST TO THE RENDERING MANAGER FOR THAT
        }

        vec2s f_MousePos = InputManager::get_single().GetCurrentMousePosition();

        PEACH_PRINT_FMT(PEACH_COL_GREEN, "mouse x : {}, y: {}", f_MousePos.x, f_MousePos.y);
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
}