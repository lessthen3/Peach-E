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
            const PeachCore::RendererType fp_RenderingBackend
        )
    {
        //Enable ANSI colour codes for windows console grumble grumble
        #if defined(_WIN32) or defined(_WIN64)
            EnableColors();
        #endif

        main_logger = make_unique<PeachCore::LogManager>();
        main_logger->Initialize(PeachCore::ThreadName::MainThread, fp_RootPath + "/logs", "MainLogger", peach_engine_console.GetConsoleLogger(), PeachCore::LogManager::LogLevel::All);
        main_logger->LogAndPrint("MainLogger successfully initialized", "PeachEngineManager", PeachCore::LogManager::LogLevel::Debug);

        if (not SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) //YEAH THIS should be here oops idk how we created a SDL window before calling init oop
        {
            main_logger->LogAndPrint(format("SDL could not initialize! ending engine program execution immediately, SDL_Error: {}", string(SDL_GetError())), "PeachEngineManager", PeachCore::LogManager::LogLevel::Fatal);
            return false;
        }
        else if (not InitializePhysFS(fp_RootPath.c_str()))
        {
            main_logger->LogAndPrint("Failed to initialize Peach Engine virtual file system, ending engine program execution immediately", "PeachEngineManager", PeachCore::LogManager::LogLevel::Fatal);
            return false;
        }
        else if (not InitializeQueues())
        {

            return false;
        }
        else if (not InitalizeManagers(fp_RootPath, fp_RenderingBackend))
        {
            main_logger->LogAndPrint("Failed to initialize Peach Engine managers, ending engine program execution immediately", "PeachEngineManager", PeachCore::LogManager::LogLevel::Fatal);
            return false;
        }

        //////////////////////////////////////////////
        // Load and Setup Plugins
        //////////////////////////////////////////////

        vector<string> f_ListOfPluginsToLoad;

        #if defined(_WIN32) or defined(_WIN64) //hard coded for now, will be dynamically loaded using a project file encoded in JSON or binary in the future
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
        ShutdownPlugins();
        SDL_Quit(); //just makes more sense to have the ShutdownPeachEngine method to do this

        return true;
    }

    bool
        GameManager::InitializePhysFS(const char* fp_RootPath)
    {
        if (not PHYSFS_init(fp_RootPath))
        {
            main_logger->LogAndPrint("Failed to initialize PhysFS: " + static_cast<string>(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())), "PeachEngineManger", PeachCore::LogManager::LogLevel::Fatal);
            return false;
        }

        // Set the writable directory to the repo root
        if (not PHYSFS_setWriteDir(fp_RootPath))
        {
            main_logger->LogAndPrint("Failed to set write directory: " + static_cast<string>(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())), "PeachEngineManger", PeachCore::LogManager::LogLevel::Fatal);
            return false;
        }

        // Mount the root directory for asset loading
        if (not PHYSFS_mount(fp_RootPath, nullptr, 1))
        {
            main_logger->LogAndPrint("Failed to set search path: " + static_cast<string>(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())), "PeachEngineManger", PeachCore::LogManager::LogLevel::Fatal);
            return false;
        }

        main_logger->LogAndPrint("PhysFS initialized at root: " + static_cast<string>(fp_RootPath), "PeachEngineManger", PeachCore::LogManager::LogLevel::Debug);
        return true;
    }

    bool
        GameManager::InitalizeManagers
        (
            const string& fp_RootPath, 
            const PeachCore::RendererType fp_RenderingBackend
        )
    {
        const string f_LogDir = fp_RootPath + "/logs";

        PeachCore::PhysicsManager2D::PhysicsWorld().Initialize(f_LogDir, peach_engine_console.GetConsoleLogger(), 0.0f, -9.8f);
        PeachCore::AudioManager::AudioPlayer().Initialize(f_LogDir, peach_engine_console.GetConsoleLogger());
        PeachCore::RenderingManager::Renderer().Initialize(fp_RenderingBackend, f_LogDir, peach_engine_console.GetConsoleLogger());
        PeachCore::ResourceManager::ResourceLoader().Initialize(f_LogDir, peach_engine_console.GetConsoleLogger());
        //PeachCore::LogManager::NetworkLogger().Initialize(f_LogDir, "NetworkLogger");
        //PeachCore::LogManager::NetworkLogger().LogAndPrint("NetworkLogger successfully initialized", "Peach-E", "debug");

        cout << "Hello World!\n"; //>w<
        main_logger->LogAndPrint("NEW ENGINE ON THE BLOCK MY SLIME", "Peach-E", PeachCore::LogManager::LogLevel::Warning);
        main_logger->LogAndPrint("Success! This Built Correctly", "Peach-E", PeachCore::LogManager::LogLevel::Trace);

        return true;
    }

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
            PeachCore::PluginInfo f_TempPlugin = {};
            PeachCore::ResourceManager::ResourceLoader().LoadPlugin(fp_ListOfPluginsToLoad[index], f_TempPlugin);

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
}