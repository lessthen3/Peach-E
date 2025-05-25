/*******************************************************************
 *                                             Peach-E v0.0.1
 *                           Created by Ranyodh Mandur - 🍑 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                     Peach-E is a free open source game engine
********************************************************************/

#define SDL_MAIN_HANDLED

#define STB_IMAGE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION

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
            const RendererType fp_RenderingBackend
        )
    {
        //Enable ANSI colour codes for windows console grumble grumble
        #if defined(_WIN32) || defined(_WIN64)
            EnableColors();
        #endif

        main_logger = make_unique<LogManager>();
        main_logger->Initialize(ThreadName::MainThread, fp_RootPath + "/logs", "MainLogger", peach_engine_console.GetConsoleLogger(), LogManager::LogLevel::All);
        main_logger->PEACH_LOG("MainLogger successfully initialized", "PeachEngineManager", LogManager::LogLevel::Debug);

        m_UserLogger = make_shared<LogManager>();
        m_UserLogger->Initialize(ThreadName::MainThread, fp_RootPath + "/logs", "UserLogger", peach_engine_console.GetConsoleLogger(), LogManager::LogLevel::All);
        m_UserLogger->PEACH_LOG("UserLogger successfully initialized", "PeachEngineManager", LogManager::LogLevel::Debug);

        if (not SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) //YEAH THIS should be here oops idk how we created a SDL window before calling init oop
        {
            main_logger->PEACH_LOG(format("SDL could not initialize! ending engine program execution immediately, SDL_Error: {}", string(SDL_GetError())), "GameManager", LogManager::LogLevel::Fatal);
            return false;
        }
        else if (not InitializePhysFS(fp_RootPath.c_str()))
        {
            main_logger->PEACH_LOG("Failed to initialize Peach Engine virtual file system, ending engine program execution immediately", "GameManager", LogManager::LogLevel::Fatal);
            return false;
        }
        else if (not InitalizeManagers(fp_RootPath, fp_RenderingBackend))
        {
            main_logger->PEACH_LOG("Failed to initialize Peach Engine managers, ending engine program execution immediately", "GameManager", LogManager::LogLevel::Fatal);
            return false;
        }
        else if (not InitializeQueues())
        {
            main_logger->PEACH_LOG("Command Queue acquisiton failed, exiting engine execution immediately", "GameManager", LogManager::LogLevel::Fatal);
            return false;
        }
        
        LoadScriptRuntime(); //WARNING: this just loads the dotnet stuff for now

        //////////////////////////////////////////////
        // Load and Setup Plugins
        //////////////////////////////////////////////

        vector<string> f_ListOfPluginsToLoad;

        #if defined(_WIN32) || defined(_WIN64) //hard coded for now, will be dynamically loaded using a project file encoded in JSON or binary in the future
            //DLL's
            f_ListOfPluginsToLoad =
            {
                fp_RootPath + "/res/plugins/SimplePlugin.dll",
                fp_RootPath + "/res/plugins/SimplePlugin2.dll"
            };
        #else //Unix systems (osx and linux)
            //SO's or dylib
            f_ListOfPluginsToLoad =
            {
            };
        #endif

        LoadPluginsFromConfigs(f_ListOfPluginsToLoad);

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
        //ShutdownPlugins();
        SDL_Quit(); //just makes more sense to have the ShutdownPeachEngine method to do this

        return true;
    }

    bool
        GameManager::InitializeQueues()
    {
        //////////////////// Get Draw Command Queue ////////////////////

        pm_DrawCommandQueue = RenderingManager::get_single().GetDrawCommandQueue();

        if (not pm_DrawCommandQueue)
        {
            main_logger->PEACH_LOG("Failed to retrieve Draw Command Queue from RenderingManager, engine cannot continue execution", "GameManager", LogManager::LogLevel::Fatal);
            return false;
        }

        main_logger->PEACH_LOG("Successfully retrieved Draw Command Queue from RenderingManager", "GameManager", LogManager::LogLevel::Info);

        //////////////////// Get Audio Command Queue ////////////////////

        pm_AudioCommandQueue = AudioManager::get_single().GetAudioCommandQueue();

        if (not pm_DrawCommandQueue)
        {
            main_logger->PEACH_LOG("Failed to retrieve Audio Command Queue from AudioManager, engine cannot continue execution", "GameManager", LogManager::LogLevel::Fatal);
            return false;
        }

        main_logger->PEACH_LOG("Successfully retrieved Audio Command Queue from AudioManaager", "GameManager", LogManager::LogLevel::Info);

        //////////////////// Get Resource Loading Command Queue ////////////////////

        pm_ResourceCommandQueue = ResourceManager::get_single().GetLoadCommandQueue();

        if (not pm_ResourceCommandQueue)
        {
            main_logger->PEACH_LOG("Failed to retrieve Resource Loading Command Queue from ResourceManager, engine cannot continue execution", "GameManager", LogManager::LogLevel::Fatal);
            return false;
        }

        main_logger->PEACH_LOG("Successfully retrieved Resource Loading Command Queue from ResourceManager", "GameManager", LogManager::LogLevel::Info);

        return true;
    }

    bool
        GameManager::InitializePhysFS(const char* fp_RootPath)
    {
        if (not PHYSFS_init(fp_RootPath))
        {
            main_logger->PEACH_LOG("Failed to initialize PhysFS: " + static_cast<string>(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())), "GameManager", LogManager::LogLevel::Fatal);
            return false;
        }
        // Set the writable directory to the repo root
        else if (not PHYSFS_setWriteDir(fp_RootPath))
        {
            main_logger->PEACH_LOG("Failed to set write directory: " + static_cast<string>(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())), "GameManager", LogManager::LogLevel::Fatal);
            return false;
        }
        // Mount the root directory for asset loading
        else if (not PHYSFS_mount(fp_RootPath, nullptr, 1))
        {
            main_logger->PEACH_LOG("Failed to set search path: " + static_cast<string>(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())), "GameManager", LogManager::LogLevel::Fatal);
            return false;
        }

        main_logger->PEACH_LOG("PhysFS initialized at root: " + static_cast<string>(fp_RootPath), "GameManager", LogManager::LogLevel::Debug);
        return true;
    }

    bool
        GameManager::InitalizeManagers
        (
            const string& fp_RootPath, 
            const RendererType fp_RenderingBackend
        )
    {
        const string f_LogDir = fp_RootPath + "/logs";

        //resource manager should be initialized first, otherwise the loading queues will be nullptr
        ResourceManager::get_single().Initialize(f_LogDir, fp_RootPath, peach_engine_console.GetConsoleLogger());
        PhysicsManager2D::get_single().Initialize(f_LogDir, peach_engine_console.GetConsoleLogger(), 0.0f, -9.8f);
        AudioManager::get_single().Initialize(f_LogDir, peach_engine_console.GetConsoleLogger());
        RenderingManager::get_single().Initialize(fp_RenderingBackend, f_LogDir, peach_engine_console.GetConsoleLogger());
        NetworkManager::get_single().InitializeNetworking(f_LogDir, peach_engine_console.GetConsoleLogger()); //stole get_single from godot style uwu

        cout << "Hello World!\n"; //>w<
        main_logger->PEACH_LOG("NEW ENGINE ON THE BLOCK MY SLIME", "Peach-E", LogManager::LogLevel::Warning);
        main_logger->PEACH_LOG("Success! This Built Correctly", "Peach-E", LogManager::LogLevel::Trace);

        return true;
    }

    bool
        GameManager::LoadScriptRuntime()
    {
        //WARNING: hard coded path for hostfxr for testing on windows rn to get things workin
        if (not ResourceManager::get_single().LoadDotNetRuntime("res/script_runtimes/win64/dotnet/hostfxr.dll", pm_DotnetContext))
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

        while (m_Running)
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

            PollUserInputEvents();  // Handle user input

            // Physics and fixed interval updates
            while (f_PhysicsAccumulator >= f_PhysicsDeltaTime)
            {
                ConstantUpdate(f_PhysicsDeltaTime);
                StepPhysicsWorldState(f_PhysicsDeltaTime);
                f_PhysicsAccumulator -= f_PhysicsDeltaTime;
            }

            // User-defined game logic updates
            while (f_GeneralUpdateAccumulator >= f_UserDefinedDeltaTime)
            {
                UpdatePlugins(f_UserDefinedDeltaTime); //run loaded plugins alongside player scripts uwu
                Update(f_UserDefinedDeltaTime);
                f_GeneralUpdateAccumulator -= f_UserDefinedDeltaTime;
            }

            if (f_RenderAccumulator >= f_RenderDeltaTime)
            {
                RenderFrame();
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
        for (int index = 0; index < fp_ListOfPluginsToLoad.size(); index++)
        {
            PluginInfo f_TempPlugin = {};
            ResourceManager::get_single().LoadPlugin(fp_ListOfPluginsToLoad[index], f_TempPlugin);

            pm_PluginInstances.emplace_back(move(f_TempPlugin.Pwugin), f_TempPlugin.Handle);
        }
    }

    void 
        GameManager::InitializePlugins()
    {
        for (auto& __plugin_info : pm_PluginInstances)
        {
            __plugin_info.Pwugin->Initialize();
        }
    }

    void 
        GameManager::UpdatePlugins(float fp_TimeSinceLastFrame)
    {
        for (auto& __plugin_info : pm_PluginInstances)
        {
            __plugin_info.Pwugin->Update(fp_TimeSinceLastFrame);
        }
    }

    void 
        GameManager::ConstantUpdatePlugins(float fp_TimeSinceLastFrame)
    {
        for (auto& __plugin_info : pm_PluginInstances)
        {
            __plugin_info.Pwugin->ConstantUpdate(fp_TimeSinceLastFrame);
        }
    }

    void 
        GameManager::ShutdownPlugins()
    {
        for (auto& __plugin_info : pm_PluginInstances)
        {
            __plugin_info.Pwugin->Shutdown(); //plugin devs better cleanup after themselves, nothing I can do to ensure safety here uwu

            if (__plugin_info.Handle != nullptr)
            {
                DYNLIB_UNLOAD(__plugin_info.Handle);
            }
        }

        pm_PluginInstances.clear(); //wait why am i clearing plugin handles before unloading them LMFAO, XXX: fixed it uwu ><
    }

    //////////////////////////////////////////////
    // Thread Stuff
    //////////////////////////////////////////////

    void
        GameManager::RenderThread()
    {
        while (true)
        {
            // Play audio
            cout << "Playing ur mom LOL...\n";
            this_thread::sleep_for(chrono::seconds(2)); // Simulate work
        }
    }

    void
        GameManager::AudioThread()
    {
        while (true)
        {
            // Play audio
            cout << "Playing audio...\n";
            this_thread::sleep_for(chrono::seconds(2)); // Simulate work
        }
    }

    void
        GameManager::ResourceThread()
    {
        while (true)
        {
            // Load resources
            cout << "Loading resources...\n";
            this_thread::sleep_for(chrono::seconds(2)); // Simulate work
        }
    }

    void
        GameManager::NetworkThread()
    {
        while (true)
        {
            // Handle network communication
            cout << "Handling network...\n";
            this_thread::sleep_for(chrono::seconds(2)); // Simulate work
        }
    }

    void
        GameManager::PhysicsThread() //processes all physics, changing structure of engine because main thread should execute scripts instead of physics calculations
    {
        while (true)
        {
            // Handle network communication
            cout << "Handling network...\n";
            this_thread::sleep_for(chrono::seconds(2)); // Simulate work
        }
    }

    bool
        GameManager::InitializeThreads() //XXX: used for kickstarting threads needed for engine execution
    {
        pm_RenderThread = thread(&GameManager::RenderThread, this);
        pm_AudioThread = thread(&GameManager::AudioThread, this);
        pm_ResourceThread = thread(&GameManager::ResourceThread, this);
        pm_NetworkThread = thread(&GameManager::NetworkThread, this);
        pm_PhysicsThread = thread(&GameManager::PhysicsThread, this);

        pm_RenderThread.detach();
        pm_AudioThread.detach();
        pm_ResourceThread.detach();
        pm_NetworkThread.detach();
        pm_PhysicsThread.detach();

        return true;
    }
}