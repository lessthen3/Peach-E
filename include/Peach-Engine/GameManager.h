/*******************************************************************
 *                                             Peach-E v0.0.1
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
#include <thread>

//SHOULD MANAGE THE ENTIRE GAME ENGINE ON THE MAIN THREAD, IM NOT SURE IF ILL MOVE ALL THE IMPORTANT CODE FROM MAIN INTO HERE TO CLEAN THINGS UP

//AND MAKE RESPONSIBILITES AND CODE IN GENERAL MORE CLEAN AND EASY TO READ

using namespace std; 

namespace PeachEngine {

    namespace PC = PeachCore;

    class GameManager 
    {
    //////////////////////////////////////////////
    // Private Destructor
    //////////////////////////////////////////////
    private:
        ~GameManager() {}

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
        GameManager() {}

        GameManager(const GameManager&) = delete;
        GameManager& operator=(const GameManager&) = delete;

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
                const string& fp_RootPath,
                const PeachCore::RendererType fp_RenderingBackend
            )
        {
            //Enable ANSI colour codes for windows console grumble grumble
            #if defined(_WIN32) || defined(_WIN64)
                EnableColors();
            #endif

            main_logger = make_unique<PC::LogManager>();
            main_logger->Initialize(fp_RootPath + "/logs", "MainLogger", peach_engine_console.GetConsoleLogger());
            main_logger->LogAndPrint("MainLogger successfully initialized", "PeachEngineManager", PeachCore::LogManager::LogLevel::Debug, "main_thread");

            if (not InitalizeManagers(fp_RootPath, fp_RenderingBackend))
            {
                main_logger->LogAndPrint("Failed to initialize Peach Engine managers, ending engine program execution immediately", "PeachEngineManager", PeachCore::LogManager::LogLevel::Fatal, "main_thread");
                return false;
            }

            if (not InitializePhysFS(fp_RootPath.c_str()))
            {
                main_logger->LogAndPrint("Failed to initialize Peach Engine virtual file system, ending engine program execution immediately", "PeachEngineManager", PeachCore::LogManager::LogLevel::Fatal, "main_thread");
                return false;
            }

            if (not SDL_Init(SDL_INIT_VIDEO))
            {
                main_logger->LogAndPrint(format("SDL could not initialize! ending engine program execution immediately, SDL_Error: {}", string(SDL_GetError())), "PeachEngineManager", PeachCore::LogManager::LogLevel::Fatal, "render_thread");
                return false;
            }

            //////////////////////////////////////////////
            // Load and Setup Plugins
            //////////////////////////////////////////////

            vector<string> f_ListOfPluginsToLoad;

            #if defined(_WIN32) || defined(_WIN64) //hard coded for now, will be dynamically loaded using a project file encoded in JSON or binary in the future
                //DLL's
                f_ListOfPluginsToLoad =
                {
                    fp_RootPath + "/plugins/SimplePlugin.dll",
                    fp_RootPath + "/plugins/SimplePlugin2.dll"
                };
            #else //Unix systems (osx and linux)
                //SO's or dylib
                f_ListOfPluginsToLoad =
                {
                };
            #endif

            LoadPluginsFromConfigs(f_ListOfPluginsToLoad);

            PeachCore::PluginManager::ManagePlugins().InitializePlugins();

            //////////////////////////////////////////////
            // Load Startup Configs
            //////////////////////////////////////////////

            LoadGameStartupConfigs(); //used for telling peach engine which scene should be booted first, along with any other relevant startup routine instructions

            return true;
        }

        void 
            StartMainGameLoop()
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
        // Shutdown and Cleanup OwO
        //////////////////////////////////////////////
        bool
            ShutdownPeachEngine()
        {
            //CLEAN-UP AND ANY CLOSING THINGS THAT SHOULD BE LOGGED TO CHECK THE STATE OF THE ENGINE AS IT EXITS
            PeachCore::PluginManager::ManagePlugins().ShutdownPlugins();
            SDL_Quit(); //just makes more sense to have the ShutdownPeachEngine method to do this

            return true;
        }

    //////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////
    private:
        //////////////////////////////////////////////
        // Thread Methods
        //////////////////////////////////////////////
        void RenderThread()
        {
            while (true)
            {
                // Play audio
                cout << "Playing ur mom LOL...\n";
                this_thread::sleep_for(chrono::milliseconds(16)); // Simulate work
            }
        }

        void AudioThread()
        {
            while (true)
            {
                // Play audio
                cout << "Playing audio...\n";
                this_thread::sleep_for(chrono::milliseconds(16)); // Simulate work
            }
        }

        void ResourceLoadingThread()
        {
            while (true)
            {
                // Load resources
                cout << "Loading resources...\n";
                this_thread::sleep_for(chrono::milliseconds(100)); // Simulate work
            }
        }

        void NetworkThread()
        {
            while (true)
            {
                // Handle network communication
                cout << "Handling network...\n";
                this_thread::sleep_for(chrono::milliseconds(16)); // Simulate work
            }
        }

        void PhysicsThread() //processes all physics, changing structure of engine because main thread should execute scripts instead of physics calculations
        {
            while (true)
            {
                // Handle network communication
                cout << "Handling network...\n";
                this_thread::sleep_for(chrono::milliseconds(16)); // Simulate work
            }
        }

        //////////////////////////////////////////////
        // Script Binding Methods
        //////////////////////////////////////////////
        
        //PYBIND11_MODULE(peach_engine, fp_Module)
        //{
        //    PythonScriptManager::Python().InitializePythonBindingsForPeachCore(fp_Module);
        //}

        //////////////////////////////////////////////
        // Engine Initialization Methods
        //////////////////////////////////////////////
        bool
            InitalizeManagers(const string& fp_RootPath, const PeachCore::RendererType fp_RenderingBackend)
        {
            const string f_LogDir = fp_RootPath + "/logs";

            PeachCore::PhysicsManager2D::PhysicsWorld().Initialize(f_LogDir, peach_engine_console.GetConsoleLogger(), 0.0f, -9.8f);
            PeachCore::PluginManager::ManagePlugins().Initialize(f_LogDir, peach_engine_console.GetConsoleLogger());
            PeachCore::AudioManager::AudioPlayer().Initialize(f_LogDir, peach_engine_console.GetConsoleLogger());
            PeachCore::RenderingManager::Renderer().Initialize(fp_RenderingBackend, f_LogDir, peach_engine_console.GetConsoleLogger());
            PeachCore::ResourceLoadingManager::ResourceLoader().Initialize(f_LogDir, peach_engine_console.GetConsoleLogger());

            //PeachCore::LogManager::NetworkLogger().Initialize(f_LogDir, "NetworkLogger");

            //PeachCore::LogManager::NetworkLogger().LogAndPrint("NetworkLogger successfully initialized", "Peach-E", "debug");

            cout << "Hello World!\n"; //>w<

            main_logger->LogAndPrint("NEW ENGINE ON THE BLOCK MY SLIME", "Peach-E", PeachCore::LogManager::LogLevel::Warning, "main_thread");

            main_logger->LogAndPrint("Success! This Built Correctly", "Peach-E", PeachCore::LogManager::LogLevel::Trace, "main_thread");

            return true;
        }

        //////////////////////////////////////////////
        // Setting Up and Setting Output Directory
        //////////////////////////////////////////////
        bool
            InitializePhysFS(const char* fp_RootPath)
        {
            if (not PHYSFS_init(fp_RootPath))
            {
                main_logger->LogAndPrint("Failed to initialize PhysFS: " + static_cast<string>(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())), "PeachEngineManger", PeachCore::LogManager::LogLevel::Fatal, "main_thread");
                return false;
            }

            // Set the writable directory to the repo root
            if (not PHYSFS_setWriteDir(fp_RootPath))
            {
                main_logger->LogAndPrint("Failed to set write directory: " + static_cast<string>(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())), "PeachEngineManger", PeachCore::LogManager::LogLevel::Fatal, "main_thread");
                return false;
            }

            // Mount the root directory for asset loading
            if (not PHYSFS_mount(fp_RootPath, nullptr, 1))
            {
                main_logger->LogAndPrint("Failed to set search path: " + static_cast<string>(PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())), "PeachEngineManger", PeachCore::LogManager::LogLevel::Fatal, "main_thread");
                return false;
            }

            main_logger->LogAndPrint("PhysFS initialized at root: " + static_cast<string>(fp_RootPath), "PeachEngineManger", PeachCore::LogManager::LogLevel::Debug, "main_thread");
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

        void IssueLoadingCommands(vector<PeachCore::LoadCommand> fp_ListOfLoadCommands)
        {

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
