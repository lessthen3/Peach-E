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

#include <thread>

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

        unique_ptr<LogManager> main_logger = nullptr;
        PeachConsole peach_engine_console;

        //////////////////// Loading/Command Queues ////////////////////

        shared_ptr<CommandQueue> pm_DrawCommandQueue = nullptr;
        shared_ptr<CommandQueue> pm_AudioCommandQueue = nullptr;
        shared_ptr<CommandQueue> pm_ResourceCommandQueue = nullptr;

        //////////////////// Plugin Stuff ////////////////////

        vector<PluginInfo> pm_PluginInstances;

        //////////////////// Script Runtimes ////////////////////

        DotnetContext pm_DotnetContext;

        //////////////////// Thread Handles ////////////////////

        thread pm_RenderThread;
        thread pm_PhysicsThread;
        thread pm_ResourceThread;
        thread pm_AudioThread;
        thread pm_NetworkThread;

        //////////////////// Scene Stuff ////////////////////

        Scene pm_CurrentScene;
        map<string, Scene> DictionaryOfAllScenesInCurrentProject;

    //////////////////////////////////////////////
    // Public Members
    //////////////////////////////////////////////
    public:
        atomic<bool> m_Running = true;

        const float USER_DEFINED_CONSTANT_UPDATE_FPS = 60.0f;
        const float USER_DEFINED_UPDATE_FPS = 60.0f;
        float        USER_DEFINED_RENDER_FPS = 120.0f; //Needs to be adjustable in-game so no const >w<

        shared_ptr<LogManager> m_UserLogger;
        shared_ptr<CommandQueue> m_UserScriptCommandQueue = nullptr; //XXX: used for submitting update commands -> GameManager from script runtimes

    //////////////////////////////////////////////
    // Public Methods
    //////////////////////////////////////////////
    public:
        bool //WIP IM NOT SURE IF INITIALIZE OPENGL SHOULD BE HERE OR ANOTHER METHOD WHATEVER
            InitializePeachEngine
            (
                const string& fp_RootPath,
                const string& fp_BootConfPath,
                const RendererType fp_RenderingBackend
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

        //////////////////// Thread Methods ////////////////////

        bool
            InitializeThreads(uint8_t fp_RequiredThreads);

        void
            RenderThread();

        void
            AudioThread();

        void
            ResourceThread();

        void
            NetworkThread();

        void
            PhysicsThread();

        //////////////////// Engine Initialization Methods ////////////////////

        bool
            InitializeManagers
            (
                const string& fp_RootPath, 
                const RendererType fp_RenderingBackend
            );

        bool
            InitializeQueues();

        bool
            InitializePhysFS(const char* fp_RootPath);

        void
            CheckForDirectoryChanges()
        {
            static map<string, PHYSFS_sint64> lastModifiedTimes;

            char** rc = PHYSFS_enumerateFiles("/");

            for (char** i = rc; *i != NULL; i++)
            {
                string fullPath = string("/") + *i;
                PHYSFS_Stat stat;

                if (PHYSFS_stat(fullPath.c_str(), &stat))
                {
                    if (lastModifiedTimes.find(fullPath) == lastModifiedTimes.end() or lastModifiedTimes[fullPath] != stat.modtime)
                    {
                        // File has changed or is new
                        //processFileChange(fullPath);
                        // Update the last modified time
                        lastModifiedTimes[fullPath] = stat.modtime;
                    }
                }
            }

            PHYSFS_freeList(rc);
        }

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

        void RenderFrame() 
        {
            cout << "Rendering frame...\n";
            this_thread::sleep_for(chrono::milliseconds(16)); // Simulate work
        }

        void StepPhysicsWorldState(float fp_FixedDeltaTime)
        {

        }

        void ConstantUpdate(float fp_FixedDeltaTime) 
        {
            cout << "Updating Physics frame...\n";
            this_thread::sleep_for(chrono::milliseconds(16)); // Simulate work
        }

        void Update(float fp_FixedDeltaTime) 
        {
            cout << "Updating frame...\n";
            this_thread::sleep_for(chrono::milliseconds(16)); // Simulate work
        }

        void PollUserInputEvents()
        {

        }

        void IssueLoadingCommands(vector<LoadCommand> fp_ListOfLoadCommands)
        {

        }

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

        //////////////////// Issue Command Call Methods ////////////////////

        void 
            PushDrawCommands //XXX: MEANT TO BE CALLED ONCE EVERY FRAME, SO WE BATCH ALL CALLS TOGETHER FOR EACH CATEGORY
            (
                const CreateDrawableData& fp_CreateData, 
                const UpdateActiveDrawableData& fp_UpdateData, 
                const DeleteDrawableData& fp_DeleteData
            )
        {
            /*if (!createData.objectIDs.empty()) {
                Command createCmd{ CommandType::CreateAsset, createData };
                commandQueue.push(createCmd);
            }
            if (!updateData.objectIDs.empty()) {
                Command updateCmd{ CommandType::UpdateAsset, updateData };
                commandQueue.push(updateCmd);
            }
            if (!deleteData.objectIDs.empty()) {
                Command deleteCmd{ CommandType::DeleteAsset, deleteData };
                commandQueue.push(deleteCmd);
            }*/
        }

        void
            PushAudioCommands //XXX: MEANT TO BE CALLED ONCE EVERY FRAME, SO WE BATCH ALL CALLS TOGETHER FOR EACH CATEGORY
            (
                const CreateDrawableData& fp_CreateData,
                const UpdateActiveDrawableData& fp_UpdateData,
                const DeleteDrawableData& fp_DeleteData
            )
        {
            /*if (!createData.objectIDs.empty()) {
                Command createCmd{ CommandType::CreateAsset, createData };
                commandQueue.push(createCmd);
            }
            if (!updateData.objectIDs.empty()) {
                Command updateCmd{ CommandType::UpdateAsset, updateData };
                commandQueue.push(updateCmd);
            }
            if (!deleteData.objectIDs.empty()) {
                Command deleteCmd{ CommandType::DeleteAsset, deleteData };
                commandQueue.push(deleteCmd);
            }*/
        }
        /*
        Needa figure out how to sync commands and loaded assets, since if the main thread asks the renderthread to draw smth or the audio thread to play a sound
        then, if the asset isnt fully loaded or present then it can cause issues, there needs to be a efficient protocol for figuring this stuff out, like a flag set or
        the ResourceManager sends an ack response to the main thread to indicate that its been loaded, and the main thread can keep a list of objects that have been
        asked to be loaded, and only after its been fully loaded, then the main thread takes it outta the waiting to be loaded list. 

        and only after that, does the main thread actually tell the other threads it can use those because i dont wanna check EVERY FRAME for EVERY ASSET
        if they exist or not lmfao.

        XXX: if a loaded packge is pushed towards a thread it means "just take this and store it for later use when instructed by the command queue"
        */
        void
            PushLoadCommands
            (
                const CreateDrawableData& fp_CreateData,
                const UpdateActiveDrawableData& fp_UpdateData,
                const DeleteDrawableData& fp_DeleteData
            )
        {
            /*if (!createData.objectIDs.empty()) {
                Command createCmd{ CommandType::CreateAsset, createData };
                commandQueue.push(createCmd);
            }
            if (!updateData.objectIDs.empty()) {
                Command updateCmd{ CommandType::UpdateAsset, updateData };
                commandQueue.push(updateCmd);
            }
            if (!deleteData.objectIDs.empty()) {
                Command deleteCmd{ CommandType::DeleteAsset, deleteData };
                commandQueue.push(deleteCmd);
            }*/
        }
    };
}
