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

#include <thread>

#ifndef __cpp_lib_jthread
//idfk freebsd 15 is weird w its clang mang
#endif

//SHOULD MANAGE THE ENTIRE GAME ENGINE ON THE MAIN THREAD, IM NOT SURE IF ILL MOVE ALL THE IMPORTANT CODE FROM MAIN INTO HERE TO CLEAN THINGS UP

//AND MAKE RESPONSIBILITES AND CODE IN GENERAL MORE CLEAN AND EASY TO READ

namespace PeachCore {

    //////////////////////////////////////////////
    // ThreadName Enum
    //////////////////////////////////////////////

    enum class ThreadName : uint8_t
    {
        MainThread = 1 << 0,
        RenderThread = 1 << 1,
        ResourceThread = 1 << 2,
        NetworkThread = 1 << 3,
        PhysicsThread = 1 << 4,
        AudioThread = 1 << 5,

        NO_THREAD = 0,
        ALL_THREADS = MainThread | RenderThread | ResourceThread | NetworkThread | PhysicsThread | AudioThread
    };

    inline
        ThreadName operator|(ThreadName fp_FuckCpp, ThreadName fp_FuckYou) //fuck C++ CoodOEOs MSelLLLSlelS Ss brb ima write C++ like java and be confused why it doesn't work uwu
        noexcept
    {
        return static_cast<ThreadName>(static_cast<uint8_t>(fp_FuckCpp) | static_cast<uint8_t>(fp_FuckYou));
    }

    inline bool 
        operator&(ThreadName fp_FuckCpp, ThreadName fp_FuckYou)
        noexcept
    {
        return static_cast<uint8_t>(fp_FuckCpp) & static_cast<uint8_t>(fp_FuckYou);
    }

    inline ThreadName
        operator^(ThreadName fp_FuckCpp, ThreadName fp_FuckYou)
        noexcept
    {
        return static_cast<ThreadName>(static_cast<uint8_t>(fp_FuckCpp) ^ static_cast<uint8_t>(fp_FuckYou));
    }

    enum class ScriptRuntimeType : uint8_t
    {
        Dotnet = 1 << 0,
        BongoJam = 1 << 1,
        Lua = 1 << 2,
        None = 0
    };

    class GameManager 
    {
    //////////////////////////////////////////////
    // Private Destructor and Constructor
    //////////////////////////////////////////////
    private:
        ~GameManager();
        GameManager() = default;

    //////////////////////////////////////////////
    // Singleton Instance
    //////////////////////////////////////////////
    public:
        static GameManager& get_single()
        {
            static GameManager peach_engine;
            return peach_engine;
        }

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

        shared_ptr<NetworkCommandPipe> pm_NetworkCommandQueue = nullptr;
        shared_ptr<PhysicsCommandPipe> pm_PhysicsCommandQueue = nullptr;

        //shared_ptr<CommandQueue> m_UserScriptCommandQueue = nullptr; //XXX: used for submitting update commands -> GameManager from script runtimes from multiple threads owo

        //////////////////// Script Runtime Contexts ////////////////////

        DotnetContext pm_DotnetContext;
        Lua::ScriptRuntime pm_LuaRuntimeContext;
        //BongoJamRuntimeContext pm_BongoJamRuntimeContext;

        vector<NativeScriptData> pm_NativeScriptPlugins; //loaded at engine startup since they run alongside the game stuff for now ig idk future ryan what do u think owo? future ryan: these are just native scripts not plugins lmfao

        //////////////////// Thread Handles ////////////////////

        thread pm_RenderThread;
        thread pm_PhysicsThread;
        thread pm_ResourceThread;
        thread pm_AudioThread;
        thread pm_NetworkThread;

        ThreadName pm_RequiredThreads = ThreadName::NO_THREAD; //required threads for execution

        //////////////////// Scene Stuff ////////////////////

        SceneTree pm_CurrentScene;
        unordered_map<string, uint64_t> pm_DictionaryOfAllScenesInCurrentProject; // Key : Scene name, Val : offset in peach binary

        //////////////////// Thread Syncro Stuff ////////////////////

        atomic<bool> m_IsRunning{ true };
        latch pm_ThreadInitializationLatch{ 4 }; //4 because thats the number of thread managers  - 1 because the resourcemanager has its own latch since the order is : resource thread first, then every other thread since those arent order sensitive uwu
        latch pm_ResourceInitializationLatch{ 1 };

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
                const ThreadName fp_RequiredThreads,
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

        //////////////////////////////////////// Peach API Functions ////////////////////////////////////////

        PEACH_STATUS_CODE
            ChangeScene(const string& fp_DesiredSceneName);

        [[nodiscard]] SceneTree*
            GetCurrentScene()
        {
            return &pm_CurrentScene;
        }

    //////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////
    private:

        //////////////////////////////////////// Thread Methods ////////////////////////////////////////

        bool
            InitializeThreads
            (
                const string& fp_RootPath,
                const uint32_t fp_InitialWindowWidth,
                const uint32_t fp_InitialWindowHeight,
                 RendererType fp_RenderingBackend, //o7
                const bool fp_Is3D = false

            );

        bool
            RetrieveQueues();

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
