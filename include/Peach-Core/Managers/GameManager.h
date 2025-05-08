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
#pragma once

#include "../../include/Peach-Core/Peach-Core.hpp"

#include <thread>

//SHOULD MANAGE THE ENTIRE GAME ENGINE ON THE MAIN THREAD, IM NOT SURE IF ILL MOVE ALL THE IMPORTANT CODE FROM MAIN INTO HERE TO CLEAN THINGS UP

//AND MAKE RESPONSIBILITES AND CODE IN GENERAL MORE CLEAN AND EASY TO READ

namespace PeachCore {

    class GameManager 
    {
    //////////////////////////////////////////////
    // Private Destructor
    //////////////////////////////////////////////
    private:
        ~GameManager() = default;

    //////////////////////////////////////////////
    // Singleton Instance
    //////////////////////////////////////////////
    public:
        static GameManager& PeachEngine()
        {
            static GameManager peach_engine;
            return peach_engine;
        }

    //////////////////////////////////////////////
    // Private Constructor
    //////////////////////////////////////////////
    private:
        GameManager() = default;

        GameManager(const GameManager&) = delete;
        GameManager& operator=(const GameManager&) = delete;

    //////////////////////////////////////////////
    // Private Members
    //////////////////////////////////////////////
    private:
        unique_ptr<LogManager> main_logger = nullptr;
        PeachConsole peach_engine_console;

        shared_ptr<CommandQueue> pm_DrawCommandQueue = nullptr;
        shared_ptr<CommandQueue> pm_AudioCommandQueue = nullptr;

        //////////////////// Plugin Stuff ////////////////////

        vector<PluginInfo> pm_PluginInstances;

        //string CurrentlySelectedRenderer = "Nothing";
        //map<string, Scene> DictionaryOfAllScenesInCurrentProject = {};

    //////////////////////////////////////////////
    // Public Members
    //////////////////////////////////////////////
    public:
        atomic<bool> m_Running = true;

        const float USER_DEFINED_CONSTANT_UPDATE_FPS = 60.0f;
        const float USER_DEFINED_UPDATE_FPS = 60.0f;
        float        USER_DEFINED_RENDER_FPS = 120.0f; //Needs to be adjustable in-game so no const >w<

    //////////////////////////////////////////////
    // Public Methods
    //////////////////////////////////////////////
    public:
        bool //WIP IM NOT SURE IF INITIALIZE OPENGL SHOULD BE HERE OR ANOTHER METHOD WHATEVER
            InitializePeachEngine
            (
                const string& fp_RootPath,
                const RendererType fp_RenderingBackend
            );

        void
            StartMainGameLoop();

        //////////////////////////////////////////////
        // Shutdown and Cleanup OwO
        //////////////////////////////////////////////
        bool
            ShutdownPeachEngine();

    //////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////
    private:
        //////////////////////////////////////////////
        // Thread Methods
        //////////////////////////////////////////////

        bool
            InitializeThreads() //XXX: used for kickstarting threads needed for engine execution
        {

            return true;
        }

        void 
            RenderThread()
        {
            while (true)
            {
                // Play audio
                cout << "Playing ur mom LOL...\n";
                this_thread::sleep_for(chrono::milliseconds(16)); // Simulate work
            }
        }

        void 
            AudioThread()
        {
            while (true)
            {
                // Play audio
                cout << "Playing audio...\n";
                this_thread::sleep_for(chrono::milliseconds(16)); // Simulate work
            }
        }

        void 
            ResourceThread()
        {
            while (true)
            {
                // Load resources
                cout << "Loading resources...\n";
                this_thread::sleep_for(chrono::milliseconds(100)); // Simulate work
            }
        }

        void 
            NetworkThread()
        {
            while (true)
            {
                // Handle network communication
                cout << "Handling network...\n";
                this_thread::sleep_for(chrono::milliseconds(16)); // Simulate work
            }
        }

        void 
            PhysicsThread() //processes all physics, changing structure of engine because main thread should execute scripts instead of physics calculations
        {
            while (true)
            {
                // Handle network communication
                cout << "Handling network...\n";
                this_thread::sleep_for(chrono::milliseconds(16)); // Simulate work
            }
        }

        //////////////////////////////////////////////
        // Engine Initialization Methods
        //////////////////////////////////////////////
        bool
            InitalizeManagers
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

        //////////////////////////////////////////////
        // Plugin Stuff
        //////////////////////////////////////////////

        void
            InitializePlugins();

        void
            UpdatePlugins(float fp_TimeSinceLastFrame);

        void
            ConstantUpdatePlugins(float fp_TimeSinceLastFrame);

        void
            ShutdownPlugins();

        void
            LoadPluginsFromConfigs(const vector<string>& fp_ListOfPluginsToLoad);

        //////////////////////////////////////////////
        // Pushing Commands To RenderingManager
        //////////////////////////////////////////////
        // 
        //MEANT TO BE CALLED ONCE EVERY FRAME, SO WE BATCH ALL CALLS TOGETHER FOR EACH CATEGORY
        void 
            PushCommands
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
