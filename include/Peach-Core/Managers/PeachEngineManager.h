#pragma once

#include "../Peach-Core.hpp"

using namespace std;

#include "LogManager.h"

#include <stdexcept>

//SHOULD MANAGE THE ENTIRE GAME ENGINE ON THE MAIN THREAD, IM NOT SURE IF ILL MOVE ALL THE IMPORTANT CODE FROM MAIN INTO HERE TO CLEAN THINGS UP

//AND MAKE RESPONSIBILITES AND CODE IN GENERAL MORE CLEAN AND EASY TO READ

namespace PeachEditor {

    class PeachEngineManager 
    {
    public:
        static PeachEngineManager& PeachEngine() {
            static PeachEngineManager peachengine;
            return peachengine;
        }

        ~PeachEngineManager()
        {

        }

    private:
        PeachEngineManager()
        {

        }

        PeachEngineManager(const PeachEngineManager&) = delete;
        PeachEngineManager& operator=(const PeachEngineManager&) = delete;


    public:
        //atomic<bool> m_Running(true);
        bool m_Running = false;

        const float USER_DEFINED_CONSTANT_UPDATE_FPS = 60.0f;
        const float USER_DEFINED_UPDATE_FPS = 60.0f;
        float        USER_DEFINED_RENDER_FPS = 120.0f; //Needs to be adjustable in-game so no const >w<

        //////////////////////////////////////////////
        // Setting Up and Setting Output Directory
        //////////////////////////////////////////////

        void 
            SetupLogManagers()
            const
        {
            PeachCore::LogManager::MainLogger().Initialize("../logs", "MainLogger");
            PeachCore::LogManager::AudioLogger().Initialize("../logs", "AudioLogger");
            PeachCore::LogManager::RenderingLogger().Initialize("../logs", "RenderingLogger");
            PeachCore::LogManager::ResourceLoadingLogger().Initialize("../logs", "ResourceLoadingLogger");
            PeachCore::LogManager::NetworkLogger().Initialize("../logs", "NetworkLogger");

            PeachCore::LogManager::MainLogger().Debug("MainLogger successfully initialized", "Peach-E");
            PeachCore::LogManager::AudioLogger().Debug("AudioLogger successfully initialized", "Peach-E");
            PeachCore::LogManager::RenderingLogger().Debug("RenderingLogger successfully initialized", "Peach-E");
            PeachCore::LogManager::ResourceLoadingLogger().Debug("ResourceLoadingLogger successfully initialized", "Peach-E");
            PeachCore::LogManager::NetworkLogger().Debug("NetworkLogger successfully initialized", "Peach-E");

            cout << "Hello World!\n"; //>w<

            PeachCore::LogManager::MainLogger().Warn("NEW ENGINE ON THE BLOCK MY SLIME", "Peach-E");

            PeachCore::LogManager::MainLogger().Trace("Success! This Built Correctly", "Peach-E");
        }

        void
            SetupVirtualFileSystem()
        {

        }

        //////////////////////////////////////////////
       // Peach Engine Startup Config Setup
       //////////////////////////////////////////////

        void LoadGameStartupConfigsFromJSON()
        {

        }

        void AdjustGameStartupJSONConfigs() //this is here for adjusting the JSON configs from the Peach Editor
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

        void StartGameLoop()
        {
            LoadGameStartupConfigsFromJSON();
            SetupLogManagers(); //only need to setup Peach-core loggers since this is run assuming the editor doesn't exist

            MainGameLoop();

            //CLEAN-UP AND ANY CLOSING THINGS THAT SHOULD BE LOGGED TO CHECK THE STATE OF THE ENGINE AS IT EXITS

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

        void 
            RunPlugins()
            const
        {
            PeachCore::PluginManager::ManagePlugins().InitializePlugins();
            PeachCore::PluginManager::ManagePlugins().UpdatePlugins(0.1f);
            PeachCore::PluginManager::ManagePlugins().ShutdownPlugins();
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
