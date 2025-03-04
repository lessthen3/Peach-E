/*******************************************************************
 *                                             Peach-E v0.1
 *                           Created by Ranyodh Mandur - � 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                         Peach-E is an open-source game engine
********************************************************************/
#pragma once

#include "../../include/Peach-Core/Peach-Core.hpp"

#include <physfs.h>

//SHOULD MANAGE THE ENTIRE GAME ENGINE ON THE MAIN THREAD, IM NOT SURE IF ILL MOVE ALL THE IMPORTANT CODE FROM MAIN INTO HERE TO CLEAN THINGS UP

//AND MAKE RESPONSIBILITES AND CODE IN GENERAL MORE CLEAN AND EASY TO READ

using namespace std; 

namespace PeachEngine {

    namespace PC = PeachCore;

    class PeachEngineManager 
    {
    //////////////////////////////////////////////
    // Private Destructor
    //////////////////////////////////////////////
    private:
        ~PeachEngineManager() {}

    //////////////////////////////////////////////
    // Singleton Instance
    //////////////////////////////////////////////
    public:
        static PeachEngineManager& PeachEngine() 
        {
            static PeachEngineManager peach_engine;
            return peach_engine;
        }

    //////////////////////////////////////////////
    // Private Constructor
    //////////////////////////////////////////////
    private:
        PeachEngineManager() {}

        PeachEngineManager(const PeachEngineManager&) = delete;
        PeachEngineManager& operator=(const PeachEngineManager&) = delete;

    //////////////////////////////////////////////
    // Private Members
    //////////////////////////////////////////////
    private:
        unique_ptr<PC::LogManager> main_logger = nullptr;
        PC::PeachConsole peach_engine_console;

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
                const char* fp_ArgVector[], 
                const vector<string>& fp_ListOfPluginsToLoad,
                const string& fp_RenderingBackend
            )
        {
            main_logger = make_unique<PC::LogManager>();
            main_logger->Initialize("..\\logs", "MainLogger", peach_engine_console.GetConsoleLogger());
            main_logger->LogAndPrint("MainLogger successfully initialized", "PeachEngineManager", "debug", "main_thread");

            LoadGameStartupConfigsFromJSON();

            if (not InitalizeManagers())
            {
                main_logger->LogAndPrint("Failed to initialize Peach Engine managers, ending engine program execution immediately", "PeachEngineManager", "fatal", "main_thread");
                return false;
            }

            if (not InitializePhysFS(fp_ArgVector[0]))
            {
                main_logger->LogAndPrint("Failed to initialize Peach Engine virtual file system, ending engine program execution immediately", "PeachEngineManager", "fatal", "main_thread");
                return false;
            }

            //////////////////////////////////////////////
            // Load and Setup Plugins
            //////////////////////////////////////////////
            #if defined(_WIN32) || defined(_WIN64)
                LoadPluginsFromConfigs(fp_ListOfPluginsToLoad); // Windows
            #else
                LoadPluginsFromConfigs(fp_ListOfPluginsToLoad); // Linux/Unix
            #endif

            PeachCore::PluginManager::ManagePlugins().InitializePlugins();

            //////////////////////////////////////////////
            // Start the Game Engine UwU
            //////////////////////////////////////////////
            //MainGameLoop();

            //////////////////////////////////////////////
            // Shutdown and Cleanup OwO
            //////////////////////////////////////////////
            //CLEAN-UP AND ANY CLOSING THINGS THAT SHOULD BE LOGGED TO CHECK THE STATE OF THE ENGINE AS IT EXITS
            PeachCore::PluginManager::ManagePlugins().ShutdownPlugins();

            return true;
        }

    //////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////
    private:
        bool
            InitalizeManagers()
        {
            PeachCore::PhysicsManager2D::PhysicsWorld().Initialize("..\\logs", peach_engine_console.GetConsoleLogger(), 0.0f, -9.8f);
            PeachCore::PluginManager::ManagePlugins().Initialize("..\\logs", peach_engine_console.GetConsoleLogger());
            PeachCore::AudioManager::AudioPlayer().Initialize("..\\logs", peach_engine_console.GetConsoleLogger());
            PeachCore::RenderingManager::Renderer().Initialize("..\\logs", peach_engine_console.GetConsoleLogger());
            PeachCore::ResourceLoadingManager::ResourceLoader().Initialize("..\\logs", peach_engine_console.GetConsoleLogger());

            //PeachCore::LogManager::NetworkLogger().Initialize("..\\logs", "NetworkLogger");

            //PeachCore::LogManager::NetworkLogger().LogAndPrint("NetworkLogger successfully initialized", "Peach-E", "debug");

            cout << "Hello World!\n"; //>w<

            main_logger->LogAndPrint("NEW ENGINE ON THE BLOCK MY SLIME", "Peach-E", "warn", "main_thread");

            main_logger->LogAndPrint("Success! This Built Correctly", "Peach-E", "trace", "main_thread");

            return true;
        }

        //////////////////////////////////////////////
        // Setting Up and Setting Output Directory
        //////////////////////////////////////////////
        bool
            InitializePhysFS(const char* argv0)
        {
            if (not PHYSFS_init(argv0))
            {
                main_logger->LogAndPrint("Failed to initialize PhysFS: " + static_cast<string>(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())), "PeachEngineManger", "fatal", "main_thread");
                return false;
            }
            //WARNING: WE ONLY USE THIS FOR DEVELOPMENT, FOR DEPLOYMENT WE NEED THIS DIRECTORY TO BE THE BASE DIR OF THE EXECUTABLE
            // Get the full path of the executable
            filesystem::path exePath = filesystem::absolute(argv0);
            filesystem::path topLevelDir = exePath.parent_path();  // Start from the executable directory

            // Traverse upwards until we find the "Peach-E" directory
            while (not topLevelDir.empty() && topLevelDir.filename() != "Peach-E") 
            {
                topLevelDir = topLevelDir.parent_path();
            }

            if (topLevelDir.empty()) 
            {
                main_logger->LogAndPrint("Failed to find the top-level directory 'Peach-E'!", "PeachEngineManger", "fatal", "main_thread");
                return false;
            }

            string rootPath = topLevelDir.string();

            // Set the writable directory to the repo root
            if (not PHYSFS_setWriteDir(rootPath.c_str())) 
            {
                main_logger->LogAndPrint("Failed to set write directory: " + static_cast<string>(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())), "PeachEngineManger", "fatal", "main_thread");
                return false;
            }

            // Mount the root directory for asset loading
            if (not PHYSFS_mount(rootPath.c_str(), nullptr, 1))
            {
                main_logger->LogAndPrint("Failed to set search path: " + static_cast<string>(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())), "PeachEngineManger", "fatal", "main_thread");
                return false;
            }

            main_logger->LogAndPrint("PhysFS initialized at root: " + rootPath, "PeachEngineManger", "debug", "main_thread");
            return true;
        }

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

        void 
            LoadGameStartupConfigsFromJSON()
        {

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

        void IssueLoadingCommands(vector<PeachCore::LoadCommand> fp_ListOfLoadCommands)
        {

        }

        void MainGameLoop()
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
                    PC::PluginManager::ManagePlugins().UpdatePlugins(f_UserDefinedDeltaTime); //run loaded plugins alongside player scripts uwu
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
        // Loading and Running Plugins From DLL'S
        //////////////////////////////////////////////

        void 
            LoadPluginsFromConfigs(const vector<string>& fp_ListOfPluginsToLoad)
            const
        {
            for (int index = 0; index < fp_ListOfPluginsToLoad.size(); index++)
            {
                PeachCore::PluginManager::ManagePlugins().LoadPlugin(fp_ListOfPluginsToLoad[index]);
            }
        }

        //////////////////////////////////////////////
        // Pushing Commands To RenderingManager
        //////////////////////////////////////////////
        // 
        //MEANT TO BE CALLED ONCE EVERY FRAME, SO WE BATCH ALL CALLS TOGETHER FOR EACH CATEGORY
        void 
            PushCommands
            (
                const PeachCore::CreateDrawableData& fp_CreateData, 
                const PeachCore::UpdateActiveDrawableData& fp_UpdateData, 
                const PeachCore::DeleteDrawableData& fp_DeleteData
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

        private:

            //string CurrentlySelectedRenderer = "Nothing";

            //map<string, Scene> DictionaryOfAllScenesInCurrentProject = {};
    };

}
