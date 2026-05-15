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
#pragma once

//////////////////////////////////////////////
// Managers
//////////////////////////////////////////////

#include "AudioManager.h"
#include "RenderingManager.h"
#include "ResourceManager.h"
#include "PhysicsManager.h"
#include "NetworkManager.h"

#include "scene_items/SceneTree.h"

#ifndef __cpp_lib_jthread
//idfk freebsd 15 is weird w its clang mang
#endif

//SHOULD MANAGE THE ENTIRE GAME ENGINE ON THE MAIN THREAD, IM NOT SURE IF ILL MOVE ALL THE IMPORTANT CODE FROM MAIN INTO HERE TO CLEAN THINGS UP

//AND MAKE RESPONSIBILITES AND CODE IN GENERAL MORE CLEAN AND EASY TO READ

namespace PeachCore {
    static inline const std::thread::id s_mainThreadId = std::this_thread::get_id();
}

namespace PeachCore::Subsystem {

    constexpr uint8_t Render = 1u << 0;
    constexpr uint8_t Network = 1u << 1;
    constexpr uint8_t Physics2D = 1u << 2;
    constexpr uint8_t Physics3D = 1u << 3;
    constexpr uint8_t Audio = 1u << 4;
}

namespace PeachCore {

    using RequiredSubsystems = uint8_t;

    // using Subsystem = uint8_t;

    enum class ScriptRuntimeType : uint8_t
    {
        Dotnet = 1 << 0,
        BongoJam = 1 << 1,
        Lua = 1 << 2,
        None = 0
    };

    struct GameManager 
    {
    public:
        ~GameManager() = default;

    //////////////////////////////////////////////
    // Singleton Instance
    //////////////////////////////////////////////
    public:
        static GameManager& get_single()
        {
            static GameManager peach_engine;
            return peach_engine;
        }

    private:
        GameManager() = default;
        
        GameManager(const GameManager&) = delete;
        GameManager& operator=(const GameManager&) = delete;
        GameManager(GameManager&&) = delete;
        GameManager& operator=(GameManager&&) = delete;

    //////////////////////////////////////////////
    // Private Members
    //////////////////////////////////////////////
    private:
        //////////////////// FPS Vars ////////////////////

        size_t USER_DEFINED_CONSTANT_UPDATE_FPS = 60u;
        size_t USER_DEFINED_POLLING_RATE = 10u; //Needs to be adjustable in-game so no const >w<

        float pm_CurrentTimeScale = 1.0f;

        //////////////////// Main Logger and Console Buffers ////////////////////

        unique_ptr<Logger> main_logger = nullptr;
        //PeachConsole peach_engine_console;

        //////////////////// Loading/Command Queues ////////////////////

        shared_ptr<LoadCommandPipe> pm_ResourceCommandQueue{ nullptr };
        shared_ptr<RenderCommandPipe> pm_RenderCommandQueue{ nullptr };
        shared_ptr<AudioCommandPipe> pm_AudioCommandQueue{ nullptr };

        //shared_ptr<CommandQueue> m_UserScriptCommandQueue = nullptr; //XXX: used for submitting update commands -> GameManager from script runtimes from multiple threads owo

        //////////////////// Script Runtime Contexts ////////////////////

        DotnetContext pm_DotnetContext;
        Lua::ScriptRuntime pm_LuaRuntimeContext;
        //BongoJamRuntimeContext pm_BongoJamRuntimeContext;

        vector<NativeScriptData> pm_NativeScriptPlugins; //loaded at engine startup since they run alongside the game stuff for now ig idk future ryan what do u think owo? future ryan: these are just native scripts not plugins lmfao

        //////////////////// Thread Handles ////////////////////

        RenderingManager pm_RenderingManager;
        PhysicsManager pm_PhysicsManager;
        ResourceManager pm_ResourceManager;
        AudioManager pm_AudioManager;
        NetworkManager pm_NetworkManager;

        RequiredSubsystems pm_ActiveSubsystems = 0; //required threads for execution

        //////////////////// Scene Stuff ////////////////////

        SceneTree pm_CurrentScene;
        unordered_map<string, uint64_t> pm_DictionaryOfAllScenesInCurrentProject; // Key : Scene name, Val : offset in peach binary

        //////////////////// Thread Syncro Stuff ////////////////////

        atomic<bool> m_IsRunning{ true };

        SDL_Window* pm_MainWindow = nullptr;
        vector<SDL_WindowID> pm_CloseWindowRequests;

    //////////////////////////////////////////////
    // Public Members
    //////////////////////////////////////////////
    public:

    //////////////////////////////////////////////
    // Public Methods
    //////////////////////////////////////////////
    public:
        [[nodiscard]] bool
            InitializePeachEngineCustom //used headless
            (
                const string& fp_RootPath,
                const RequiredSubsystems fp_RequiredSubsystems,
                const RendererType fp_RenderingBackend,
                const uint32_t fp_StartingWindowWidth,
                const uint32_t fp_StartingWindowHeight,
                bool fp_IsSegfaultHandled = false
            );

        bool
            InitializePeachEngine //ran from engine uwu
            (
                const string& fp_RootPath
            );

        void
            StartMainGameLoop();

        //////////////////////////////////////// Shutdown and Cleanup OwO ////////////////////////////////////////

        bool
            ShutdownPeachEngine();

        //////////////////////////////////////// Thread Panic ////////////////////////////////////////

        void
            ThreadPanicShutdown(PEACH_STATUS_CODE fp_PanicCode)
        {
            PEACH_PRINT_FMT(PEACH_COL_BRIGHT_MAGENTA, "shutting down engine due to thread panic with code: {}", static_cast<int>(fp_PanicCode));
            m_IsRunning.store(false, std::memory_order_release);
        }

        //////////////////////////////////////// Peach API Functions ////////////////////////////////////////

        PEACH_STATUS_CODE
            ChangeScene(const string& fp_DesiredSceneName);

        [[nodiscard]] SceneTree*
            GetCurrentScene()
        {
            return &pm_CurrentScene;
        }

        #ifdef PEACH_PLATFORM_WASM
            void 
                ___________________WasmFrameCallback(); //that should be clear enough that this is private owo
        #endif

    //////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////
    private:

        //////////////////////////////////////// Thread Methods ////////////////////////////////////////

        PEACH_STATUS_CODE
            InitializeThreads //probs should use uint32_t flags instead, since this will be read from a binary or peach.boot.override file owo
            (
                const string& fp_RootPath,
                const uint32_t fp_InitialWindowWidth,
                const uint32_t fp_InitialWindowHeight,
                RendererType fp_RenderingBackend, //o7
                const bool fp_IsRendering3D = false
            );

        bool
            InitializePhysFS(const char* fp_RootPath);

        //////////////////////////////////////// Engine Initialization Methods ////////////////////////////////////////

        bool 
            LoadGameStartupConfigs() //This method should be able to load configs from JSON or some other binary fmt::format that cereal supports
        {

            return true;
        }

        bool
            SetupScriptRuntime
            (
                const string& fp_BootConfPath,
                const uint8_t fp_RequiredScriptRuntimes
            ); 

        //////////////////////////////////////////////
        // Game Loop Methods
        //////////////////////////////////////////////

        void
            CallUpdate(double fp_MilisecondsSinceLastCall);

        void
            CallConstantUpdate(double fp_FixedDeltaTime);

        //////////////////////////////////////////////
        // Window Stuff
        //////////////////////////////////////////////

        void
            PollUserInputEvents();

        //////////////////////////////////////// Plugin Stuff ////////////////////////////////////////

        //void
        //    InitializePlugins()
        //    const;

        //void
        //    UpdatePlugins(float fp_TimeSinceLastFrame)
        //    const;

        //void
        //    ConstantUpdatePlugins(float fp_TimeSinceLastFrame)
        //    const;

        //void
        //    ShutdownPlugins();

        //void
        //    LoadPluginsFromConfigs(const vector<string>& fp_ListOfPluginsToLoad);
    };
}
