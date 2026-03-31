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
#pragma once

//////////////////////////////////////////////
// Managers
//////////////////////////////////////////////

#include "AudioManager.h"
#include "RenderingManager.h"
#include "ResourceManager.h"
#include "PhysicsManager.h"
#include "NetworkManager.h"

//SHOULD MANAGE THE ENTIRE GAME ENGINE ON THE MAIN THREAD, IM NOT SURE IF ILL MOVE ALL THE IMPORTANT CODE FROM MAIN INTO HERE TO CLEAN THINGS UP

//AND MAKE RESPONSIBILITES AND CODE IN GENERAL MORE CLEAN AND EASY TO READ

namespace PeachCore {

    //////////////////////////////////////////////
    // ThreadName Enum
    //////////////////////////////////////////////

    enum ThreadName : uint8_t
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


    enum class ScriptRuntimeType : uint8_t
    {
        Dotnet = 1 <<0,
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
        ~GameManager() = default;
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

        float USER_DEFINED_CONSTANT_UPDATE_FPS = 60.0f;
        float USER_DEFINED_UPDATE_FPS = 60.0f;
        float USER_DEFINED_RENDER_FPS = 10.0f; //Needs to be adjustable in-game so no const >w<

        //////////////////// Main Logger and Console Buffers ////////////////////

        unique_ptr<Logger> main_logger = nullptr;
        //PeachConsole peach_engine_console;

        //////////////////// Loading/Command Queues ////////////////////

        shared_ptr<LoadCommandPipe> pm_ResourceCommandQueue{ nullptr };

        shared_ptr<RenderCommandPipe> pm_RenderCommandQueue{ nullptr };

        shared_ptr<AudioCommandPipe> pm_AudioCommandQueue{ nullptr };

        shared_ptr<NetworkCommandPipe> pm_NetworkCommandQueue = nullptr;
        shared_ptr<PhysicsCommandPipe> pm_PhysicsCommandQueue = nullptr;

        //shared_ptr<CommandQueue> m_UserScriptCommandQueue = nullptr; //XXX: used for submitting update commands -> GameManager from script runtimes

        //////////////////// Plugin Stuff ////////////////////

        vector<NativeScriptData> pm_NativeScriptPlugins; //loaded at engine startup since they run alongside the game stuff for now ig idk future ryan what do u think owo? future ryan:

        //////////////////// Script Runtime Contexts ////////////////////

        DotnetContext pm_DotnetContext;
        Lua::ScriptRuntime pm_LuaRuntimeContext;
        //BongoJamRuntimeContext pm_BongoJamRuntimeContext;

        //////////////////// Thread Handles ////////////////////

        jthread pm_RenderThread;
        jthread pm_PhysicsThread;
        jthread pm_ResourceThread;
        jthread pm_AudioThread;
        jthread pm_NetworkThread;

        uint8_t pm_RequiredThreads = 0; //required threads for execution

        //////////////////// Scene Stuff ////////////////////

        SceneTree pm_CurrentScene;
        unordered_map<string, uint64_t> pm_DictionaryOfAllScenesInCurrentProject; // Key : Scene name, Val : offset in peach binary

        //////////////////// Thread Syncro Stuff ////////////////////

        atomic<bool> m_IsRunning{ true };
        latch pm_ThreadInitializationLatch{ 4 }; //4 because thats the number of thread managers  - 1 because the resourcemanager has its own latch since the order is : resource thread first, then every other thread since those arent order sensitive uwu
        latch pm_ResourceInitializationLatch{ 1 };

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
                const uint8_t fp_RequiredThreads,
                const RendererType fp_RenderingBackend,
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
            GetCurrentScene();

    //////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////
    private:

        static void
            SegFaultHandler(int fp_Signal) //primitive segfault handler
        {
            PRINT_ERROR(fmt::format("[!] Crash signal received: {}, __FATAL__SEGMENTATION__FAULT__", fp_Signal));
            // possibly notify watchdog or dump stack trace
            exit(FATAL_SEGMENTATION_FAULT); //clean exit so everything calls their destructors
        }

        //////////////////////////////////////// Thread Methods ////////////////////////////////////////

        bool
            InitializeThreads
            (
                const string& fp_RootPath,
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

        //////////////////////////////////////// Plugin Stuff ////////////////////////////////////////

        void
            InitializePlugins()
            const;

        void
            UpdatePlugins(float fp_TimeSinceLastFrame)
            const;

        void
            ConstantUpdatePlugins(float fp_TimeSinceLastFrame)
            const;

        void
            ShutdownPlugins();

        void
            LoadPluginsFromConfigs(const vector<string>& fp_ListOfPluginsToLoad);
    };
}
