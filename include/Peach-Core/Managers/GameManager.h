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

#include "../Peach-Core.hpp"

#include <csignal>

//SHOULD MANAGE THE ENTIRE GAME ENGINE ON THE MAIN THREAD, IM NOT SURE IF ILL MOVE ALL THE IMPORTANT CODE FROM MAIN INTO HERE TO CLEAN THINGS UP

//AND MAKE RESPONSIBILITES AND CODE IN GENERAL MORE CLEAN AND EASY TO READ

namespace PeachCore {

    enum ScriptRuntimeType : uint8_t
    {
        Dotnet = 1 <<0,
        Python = 1 << 1,
        Lua = 1 << 2
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
        //////////////////// Main Logger and Console Buffers ////////////////////

        unique_ptr<Logger> main_logger = nullptr;
        PeachConsole peach_engine_console;

        //////////////////// Loading/Command Queues ////////////////////

        shared_ptr<moodycamel::ReaderWriterQueue<LoadCommand, TESTING_CAMEL_QUEUE_SIZE>> pm_ResourceCommandQueue = nullptr;
        shared_ptr<moodycamel::ReaderWriterQueue<RenderCommand, TESTING_CAMEL_QUEUE_SIZE>> pm_RenderCommandQueue = nullptr;

        shared_ptr<moodycamel::ReaderWriterQueue<RenderCommand, TESTING_CAMEL_QUEUE_SIZE>> pm_AudioCommandQueue = nullptr;

        shared_ptr<moodycamel::ReaderWriterQueue<RenderCommand, TESTING_CAMEL_QUEUE_SIZE>> pm_NetworkCommandQueue = nullptr;
        shared_ptr<moodycamel::ReaderWriterQueue<RenderCommand, TESTING_CAMEL_QUEUE_SIZE>> pm_PhysicsCommandQueue = nullptr;

        //shared_ptr<CommandQueue> m_UserScriptCommandQueue = nullptr; //XXX: used for submitting update commands -> GameManager from script runtimes

        //////////////////// Plugin Stuff ////////////////////

        vector<PluginInfo> pm_PluginInstances;

        //////////////////// Script Runtimes ////////////////////

        DotnetContext pm_DotnetContext;

        //////////////////// Thread Handles ////////////////////

        jthread pm_RenderThread;
        jthread pm_PhysicsThread;
        jthread pm_ResourceThread;
        jthread pm_AudioThread;
        jthread pm_NetworkThread;

        //////////////////// Scene Stuff ////////////////////

        Scene pm_CurrentScene;
        map<string, Scene> DictionaryOfAllScenesInCurrentProject;

        atomic<bool> m_IsRunning = true;

    //////////////////////////////////////////////
    // Public Members
    //////////////////////////////////////////////
    public:
        const float USER_DEFINED_CONSTANT_UPDATE_FPS = 60.0f;
        const float USER_DEFINED_UPDATE_FPS = 60.0f;
        float        USER_DEFINED_RENDER_FPS = 10.0f; //Needs to be adjustable in-game so no const >w<

        shared_ptr<Logger> m_UserLogger;

    //////////////////////////////////////////////
    // Public Methods
    //////////////////////////////////////////////
    public:
        bool //WIP IM NOT SURE IF INITIALIZE OPENGL SHOULD BE HERE OR ANOTHER METHOD WHATEVER
            InitializePeachEngine
            (
                const string& fp_RootPath,
                const string& fp_BootConfPath,
                const RendererType fp_RenderingBackend,
                bool fp_IsSegfaultHandled = false
            );

        bool 
            LoadScriptRuntime
            (
                const string& fp_BootConfPath,
                const uint8_t fp_RequiredScriptRuntimes
            ); //WARNING: this is public for testing

        void
            StartMainGameLoop();

        //////////////////// Shutdown and Cleanup OwO ////////////////////

        bool
            ShutdownPeachEngine();

    //////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////
    private:

        static void
            SegFaultHandler(int fp_Signal) //primitive segfault handler
        {
            PrintError(format("[!] Crash signal received: {}, FATAL_SEGMENTATION_FAULT", fp_Signal));
            // possibly notify watchdog or dump stack trace
            exit(FATAL_SEGMENTATION_FAULT); //clean exit so everything calls their destructors
        }

        //////////////////// Thread Methods ////////////////////


        //////////////////// Engine Initialization Methods ////////////////////

        bool
            InitializeThreads
            (
                const string& fp_RootPath,
                uint8_t fp_RequiredThreads,
                 RendererType fp_RenderingBackend //ONLY HERE FOR TESTING SHOULD BE DEDUCED FROM PROJECT FILE
            );

        bool
            RetrieveQueues();

        bool
            InitializePhysFS(const char* fp_RootPath);

       //////////////////////////////////////////////
       // Peach Engine Startup Config Setup
       //////////////////////////////////////////////

        bool 
            LoadGameStartupConfigs() //This method should be able to load configs from JSON or some other binary format that cereal supports
        {

            return true;
        }

        //////////////////////////////////////////////
        // Game Loop Methods
        //////////////////////////////////////////////

        void
            RequestRender();

        void
            RequestPhysicsWorldStep();

        void
            CallUpdate(double fp_MilisecondsSinceLastCall);

        void
            CallConstantUpdate(double fp_FixedDeltaTime);

        //////////////////// Plugin Stuff ////////////////////

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
