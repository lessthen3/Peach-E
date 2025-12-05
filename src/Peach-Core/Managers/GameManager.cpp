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
#define SDL_MAIN_HANDLED

#define STB_IMAGE_IMPLEMENTATION

#define VOLK_IMPLEMENTATION
#define VMA_IMPLEMENTATION

#define MINIAUDIO_IMPLEMENTATION

#include "../../include/Peach-Core/Managers/GameManager.h"

namespace PeachCore
{
    //////////////////////////////////////////////
    // Initialization, Startup, and Shutdown/Cleanup OwO
    //////////////////////////////////////////////
    bool 
        GameManager::InitializePeachEngine
        (
            const string& fp_RootPath,
            const string& fp_BootConfPath,
            const RendererType fp_RenderingBackend,
            bool fp_IsSegfaultHandled
        )
    {
        //////////////////// Use Peach-E default Segfault Handler ////////////////////

        if(not fp_IsSegfaultHandled)
        {
            signal(SIGSEGV, GameManager::SegFaultHandler); //XXX: used for trying to close and flush logs on seg fault
        }

        //////////////////// Enable ANSI colour codes for windows console grumble grumble ////////////////////

        #if defined(_WIN32) || defined(_WIN64) && defined(PEACH_USING_OS_TERMINAL)
            EnableColors();
        #endif

        //////////////////// Logger Initialization ////////////////////

        main_logger = make_unique<Logger>();
        main_logger->Initialize(ThreadName::MainThread, fp_RootPath + "/logs", "MainLogger", Logger::LogLevel::ALL_LOGS);
        main_logger->Debug("MainLogger successfully initialized", "PeachEngineManager");

        m_UserLogger = make_shared<Logger>();
        m_UserLogger->Initialize(ThreadName::MainThread, fp_RootPath + "/logs", "UserLogger", Logger::LogLevel::ALL_LOGS);
        m_UserLogger->Debug("UserLogger successfully initialized", "PeachEngineManager");

        //////////////////// Initialize Subsystems ////////////////////

        if (not InitializePhysFS(fp_RootPath.c_str()))
        {
            main_logger->Fatal("Failed to initialize Peach Engine virtual file system, ending engine program execution immediately", "GameManager");
            return false;
        }
        else if (sodium_init() < 0)
        {
            main_logger->Fatal("Sodium library couldn't be initialized, it is not safe to use.", "GameManager");
            return false;
        }
        else if (not InitializeThreads(fp_RootPath, ThreadName::RenderThread | ThreadName::ResourceThread, fp_RenderingBackend))
        {
            main_logger->Fatal("Failed to initialize Peach Engine managers, ending engine program execution immediately", "GameManager");
            return false;
        }
        else if (not RetrieveQueues())
        {
            main_logger->Fatal("Command Queue acquisiton failed, exiting engine execution immediately", "GameManager");
            return false;
        }

        //LoadScriptRuntime(fp_BootConfPath, ScriptRuntimeType::Dotnet); //WARNING: this just loads the dotnet stuff for now

        //////////////////////////////////////////////
        // Load and Setup Plugins
        //////////////////////////////////////////////

        vector<string> f_ListOfPluginsToLoad;

        #if defined(_WIN32) || defined(_WIN64) //hard coded for now, will be dynamically loaded using a project file encoded in JSON or binary in the future
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
        GameManager::ShutdownPeachEngine()
    {
        //CLEAN-UP AND ANY CLOSING THINGS THAT SHOULD BE LOGGED TO CHECK THE STATE OF THE ENGINE AS IT EXITS
        
        //idk how issued commands will work here when the threads are joined uwu

        //pm_AudioThread.join();
        //pm_NetworkThread.join();
        //pm_PhysicsThread.join();

        //ShutdownPlugins();
        //RenderingManager::get_single().ForceQuit();
        ResourceManager::get_single().ShutdownResourceManager();

        pm_ResourceThread.join();
        //pm_RenderThread.join();

        return true;
    }

    bool
        GameManager::RetrieveQueues()
    {
        //////////////////// Get Resource Loading Command Queue ////////////////////

        pm_ResourceCommandQueue = ResourceManager::get_single().GetLoadCommandQueue();

        if (not pm_ResourceCommandQueue)
        {
            main_logger->Fatal("Failed to retrieve Resource Loading Command Queue from ResourceManager, engine cannot continue execution", "GameManager");
            return false;
        }

        main_logger->Info("Successfully retrieved Resource Loading Command Queue from ResourceManager", "GameManager");

        //////////////////// Get Draw Command Queue ////////////////////

        pm_RenderCommandQueue = RenderingManager::get_single().GetDrawCommandQueue();

        if (not pm_RenderCommandQueue)
        {
            main_logger->Fatal("Failed to retrieve Draw Command Queue from RenderingManager, engine cannot continue execution", "GameManager");
            return false;
        }

        main_logger->Info("Successfully retrieved Draw Command Queue from RenderingManager", "GameManager");

        //////////////////// Get Audio Command Queue ////////////////////

        //pm_AudioCommandQueue = AudioManager::get_single().GetAudioCommandQueue();

        //if (not pm_AudioCommandQueue)
        //{
        //    main_logger->Fatal("Failed to retrieve Audio Command Queue from AudioManager, engine cannot continue execution", "GameManager");
        //    return false;
        //}

        //main_logger->Info("Successfully retrieved Audio Command Queue from AudioManaager", "GameManager");

        //////////////////// Get Resource Loading Command Queue ////////////////////

        //pm_NetworkCommandQueue = NetworkManager::get_single().GetNetworkCommandQueue();

        //if (not pm_NetworkCommandQueue)
        //{
        //    main_logger->Fatal("Failed to retrieve Resource Loading Command Queue from ResourceManager, engine cannot continue execution", "GameManager");
        //    return false;
        //}

        //main_logger->Info("Successfully retrieved Resource Loading Command Queue from ResourceManager", "GameManager");

        //////////////////// Get Resource Loading Command Queue ////////////////////

        //pm_PhysicsCommandQueue = PhysicsManager::get_single().GetPhysicsCommandQueue();

        //if (not pm_PhysicsCommandQueue)
        //{
        //    main_logger->Fatal("Failed to retrieve Resource Loading Command Queue from ResourceManager, engine cannot continue execution", "GameManager");
        //    return false;
        //}

        //main_logger->Info("Successfully retrieved Resource Loading Command Queue from ResourceManager", "GameManager");

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
            uint8_t fp_RequiredThreads,
            RendererType fp_RenderingBackend //ONLY HERE FOR TESTING SHOULD BE DEDUCED FROM PROJECT FILE
        )
    {
        const string f_LogDir = fp_RootPath + "/logs";

        //IMPORTANT: resource thread needs to be initialized first so queues get created properly
        //Lazy initialization is used for everything since it isnt guaranteed that all threads will be active, only if a node is needed for a corresponding thread then the thread is started
        //otherwise we just leave it be

        if (fp_RequiredThreads & ThreadName::ResourceThread)
        {
            pm_ResourceThread = jthread(&ResourceManager::ResourceLoop, &ResourceManager::get_single(), f_LogDir, fp_RootPath);
        }

        ResourceManager::get_single().WaitUntilInitialized();

        if (fp_RequiredThreads & ThreadName::RenderThread)
        {
            pm_RenderThread = jthread(&RenderingManager::RenderLoop, &RenderingManager::get_single(), fp_RenderingBackend, f_LogDir);
        }

        //if (fp_RequiredThreads & ThreadName::AudioThread)
        //{
        //    pm_AudioThread = thread(&GameManager::AudioThread, this);
        //}
        //if (fp_RequiredThreads & ThreadName::NetworkThread)
        //{
        //    pm_NetworkThread = thread(&GameManager::NetworkThread, this);
        //}
        //if (fp_RequiredThreads & ThreadName::PhysicsThread)
        //{
        //    pm_PhysicsThread = thread(&GameManager::PhysicsThread, this);
        //}

        //PhysicsManager2D::get_single().Initialize(f_LogDir, peach_engine_console.GetConsoleLogger(), 0.0f, -9.8f);
        //AudioManager::get_single().Initialize(f_LogDir, peach_engine_console.GetConsoleLogger());
        //NetworkManager::get_single().InitializeNetworking(f_LogDir, peach_engine_console.GetConsoleLogger()); //stole get_single from godot style uwu

        cout << "Hello World!\n"; //>w<
        main_logger->Warning("NEW ENGINE ON THE BLOCK MY SLIME", "Peach-E");
        main_logger->Trace("Success! This Built Correctly", "Peach-E");

        return true;
    }

    bool
        GameManager::LoadScriptRuntime
        (
            const string& fp_BootConfPath,
            const uint8_t fp_RequiredScriptRuntimes
        )
    {
        if (fp_RequiredScriptRuntimes & ScriptRuntimeType::Dotnet and not ResourceManager::get_single().LoadDotNetRuntime(fp_BootConfPath, pm_DotnetContext))
        {

            return false;
        }
        if (fp_RequiredScriptRuntimes & ScriptRuntimeType::Python and not ResourceManager::get_single().LoadPythonRuntime())
        {

            return false;
        }
        if (fp_RequiredScriptRuntimes & ScriptRuntimeType::Lua and not ResourceManager::get_single().LoadLuaRuntime())
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
        const float f_PhysicsDeltaTime = 1.0f / USER_DEFINED_CONSTANT_UPDATE_FPS;  // Fixed physics update rate 
        const float f_UserDefinedDeltaTime = 1.0f / USER_DEFINED_UPDATE_FPS;  // User-defined Update() rate
        float f_RenderDeltaTime = 1.0f / USER_DEFINED_RENDER_FPS;  // Should be variable to allow dynamic adjustment in-game

        float f_PhysicsAccumulator = 0.0f;
        float f_GeneralUpdateAccumulator = 0.0f;
        float f_RenderAccumulator = 0.0f;

        auto f_CurrentTime = chrono::high_resolution_clock::now();

        ///TODO: log whenever frametime is running late in debug
        while (m_IsRunning.load(std::memory_order_acquire))
        {
            auto f_NewTime = chrono::high_resolution_clock::now();
            float f_FrameTime = chrono::duration<float>(f_NewTime - f_CurrentTime).count();
            f_CurrentTime = f_NewTime;

            // Prevent spiral of death by clamping frame time, frames will be skipped, but if you're already this behind then thats the least of your problems lmao
            if (f_FrameTime > 0.25)
            {
                f_FrameTime = 0.25;
            }

            f_PhysicsAccumulator += f_FrameTime;
            f_GeneralUpdateAccumulator += f_FrameTime;
            f_RenderAccumulator += f_FrameTime;

            ///WARNING: IF THE ENGINE FALLS FAR ENOUGH BEHIND IT WILL STEP ALL PHYSICS FRAMES FIRST THEN STEP UPDATES WHICH IS UH NOT IDEAL
            // Physics and fixed interval updates
            //while (f_PhysicsAccumulator >= f_PhysicsDeltaTime)
            //{
            //    NotifyPhysicsThread(f_PhysicsDeltaTime);
            //    f_PhysicsAccumulator -= f_PhysicsDeltaTime;
            //}

            // User-defined game logic updates
            //while (f_GeneralUpdateAccumulator >= f_UserDefinedDeltaTime)
            //{
            //    UpdatePlugins(f_UserDefinedDeltaTime); //run loaded plugins alongside player scripts uwu
            //    Update(f_UserDefinedDeltaTime);
            //    f_GeneralUpdateAccumulator -= f_UserDefinedDeltaTime;
            //}

            if (f_RenderAccumulator >= f_RenderDeltaTime)
            {
                if(RenderingManager::get_single().IsActive())
                {
                    RequestRender(); //tells the render thread to do smth w a flag
                }
                else
                {
                    break;
                }

                f_RenderAccumulator -= f_RenderDeltaTime;
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
            PluginData f_TempPlugin = {};
            ResourceManager::get_single().LoadPlugin(lv_PluginPath, f_TempPlugin);

            pm_PluginInstances.emplace_back(move(f_TempPlugin.Pwugin), f_TempPlugin.Handle);
        }
    }

    void 
        GameManager::InitializePlugins()
        const
    {
        for (auto& lv_PluginInfo : pm_PluginInstances)
        {
            lv_PluginInfo.Pwugin->Initialize();
        }
    }

    void 
        GameManager::UpdatePlugins(float fp_TimeSinceLastFrame)
        const
    {
        for (auto& lv_PluginInfo : pm_PluginInstances)
        {
            lv_PluginInfo.Pwugin->Update(fp_TimeSinceLastFrame);
        }
    }

    void 
        GameManager::ConstantUpdatePlugins(float fp_TimeSinceLastFrame)
        const
    {
        for (auto& lv_PluginInfo : pm_PluginInstances)
        {
            lv_PluginInfo.Pwugin->ConstantUpdate(fp_TimeSinceLastFrame);
        }
    }

    void 
        GameManager::ShutdownPlugins()
    {
        for (auto& lv_PluginInfo : pm_PluginInstances)
        {
            lv_PluginInfo.Pwugin->Shutdown(); //plugin devs better cleanup after themselves, nothing I can do to ensure safety here uwu

            if (lv_PluginInfo.Handle != nullptr)
            {
                DYNLIB_UNLOAD(lv_PluginInfo.Handle);
            }

            lv_PluginInfo.Pwugin.reset(); //clear plugin and let it delete but should change this to be explicit and not inside the plugin itself shutdown is sufficient tbh
        }

        pm_PluginInstances.clear(); //wait why am i clearing plugin handles before unloading them LMFAO, XXX: fixed it uwu ><
    }

    //////////////////////////////////////////////
    // Core Runner Functions
    //////////////////////////////////////////////

    void
        GameManager::RequestRender()
    {
        RenderingManager::get_single().pm_ShouldRender = true;
    }

    void
        GameManager::RequestPhysicsWorldStep()
    {
        cout << "Updating Physics frame...\n";
    }

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
}