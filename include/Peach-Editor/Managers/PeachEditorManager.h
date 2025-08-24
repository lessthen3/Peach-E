/*******************************************************************
 *                                        Peach Editor v0.0.7
 *                           Created by Ranyodh Mandur - 🍑 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *              Peach Editor is a free open source editor for Peach-E
********************************************************************/
#pragma once

#include "Managers/GameManager.h"
#include "PeachEditorRenderingManager.h"
#include "../Editor/PeachProject.h"

#include "../Editor/ShaderCompilerUtils.h"
#include "../Editor/LangUtils.h"

namespace PeachEditor{

    class PeachEditorManager
    {
    //////////////////////////////////////////////
    // Destructor
    //////////////////////////////////////////////
    public:
        ~PeachEditorManager() {}

        PeachEditorManager(const PeachEditorManager&) = delete;
        PeachEditorManager& operator=(const PeachEditorManager&) = delete;

    //////////////////////////////////////////////
    // Private Constructor
    //////////////////////////////////////////////
    private:
        PeachEditorManager() = default;

    //////////////////////////////////////////////
    // Private Members
    //////////////////////////////////////////////
    private:
        shared_ptr<PeachCore::Console> pm_PeachEditorConsole = nullptr;
        shared_ptr<PeachCore::LogManager> main_editor_logger = nullptr;

        shared_ptr<PeachCore::CommandQueue> pm_PeachEditorRenderingManagersCommandQueue = nullptr;
        shared_ptr<PeachCore::LoadingQueue> pm_PeachEditorDrawableResourceLoadingQueue = nullptr;

        shared_ptr<PeachCore::CommandQueue> pm_AudioManagersCommandQueue = nullptr; //lifetime is tied to renderingmanager so fuck u main thread, if renderingmanager says commandqueue is out, command queue is out
        shared_ptr<PeachCore::LoadingQueue> pm_AudioResourceLoadingQueue = nullptr; //used to push load commands that are destined for AudioManager

        DotnetConfigs pm_DotnetConfiguration;

    //////////////////////////////////////////////
    // Public Members
    //////////////////////////////////////////////
    public:
        static PeachEditorManager& get_single() 
        {
            static PeachEditorManager peach_editor;
            return peach_editor;
        }

        static atomic<bool> m_IsRunning;

    //////////////////////////////////////////////
    // Public Methods
    //////////////////////////////////////////////

    public:
        ////////////////////////////////////////////////
        // Start Main Loop
        ////////////////////////////////////////////////
        void
            StartPeachEditorMainLoop()
            const
        {
            auto peach_engine = &PeachCore::GameManager::get_single();
            auto editor_renderer = &PeachEditor::PeachEditorRenderingManager::get_single();

            bool f_IsEditorOpen = true;

            vector<SDL_WindowID> f_CloseWindowRequests;

            while (f_IsEditorOpen)
            {
                this_thread::sleep_for(chrono::milliseconds(200)); //60 fps oh i just realized the fps flickers by 1 because the floating point conversion isnt exact
                PeachCore::InputManager::get_single().PollEvents();
                editor_renderer->RenderFrame(&f_IsEditorOpen);

                PeachCore::InputManager::get_single().GetWindowCloseRequests(f_CloseWindowRequests);

                if (f_CloseWindowRequests.size() > 0)
                {
                    for(const auto& lv_Window : f_CloseWindowRequests)
                    {
                        SDL_DestroyWindow(SDL_GetWindowFromID(lv_Window));
                    }

                    break; //assuming main window is closed, WARNING: THIS IS ONLY FOR TESTING DOESNT WORK FOR MULTI WINDOW SETUPS
                }
                glm::vec2 f_MousePos = PeachCore::InputManager::get_single().GetCurrentMousePosition();

                PeachCore::Print(format("mouse x : {}, y: {}", f_MousePos.x, f_MousePos.y), PeachCore::Colours::Green);

            }

            editor_renderer->Shutdown();
            peach_engine->ShutdownPeachEngine();

            main_editor_logger->LogAndPrint("Exit Success!", "Peach-E", PeachCore::LogManager::LogLevel::Debug);
        }

        bool
            SetupInternalManagers(const string& fp_LogDir)
        {
            
            if(not PeachEditorRenderingManager::get_single().Initialize(fp_LogDir, pm_PeachEditorConsole))
            {
                main_editor_logger->LogAndPrint("Initialization error: PeachEditorRenderer failed to initialize properly, exiting program execution immediately", "PeachEditorManager", PeachCore::LogManager::LogLevel::Fatal);
                return false;
            }

            return true;
        }

        bool
            InitializePeachEditor(const string& fp_RootPath) //XXX: idk this method seems kinda weird idk how im gonna manage error codes but w/e thats for future me to handle UwU
        {
            //////////////////// Create Console for Entire Editor ////////////////////

            pm_PeachEditorConsole = make_shared<PeachCore::Console>();

            //////////////////// Create Main Thread Logger ////////////////////

            const string f_LogDir = fp_RootPath + "/logs";

            main_editor_logger = make_unique<PeachCore::LogManager>();

            if (not main_editor_logger->Initialize(PeachCore::ThreadName::MainThread, f_LogDir, "PeachEditorManager", pm_PeachEditorConsole, PeachCore::LogManager::LogLevel::All))
            {
                PeachCore::PrintError("Initialization error: Was not able to initialize PeachEditorManager's main logger");
                return false;
            }

            main_editor_logger->LogAndPrint("Main editor logger successfully initialized", "PeachEditorManager", PeachCore::LogManager::LogLevel::Debug);

            //////////////////// Get Local HostFxr Path and Validate Dotnet Exists ////////////////////

            DotnetUtils::AssertDotnetExists(); //dummy call but should actually make lmfao

            string f_HostFxrPath;
            DotnetUtils::GetHostFxrLocalPath(&f_HostFxrPath, main_editor_logger.get());

            //////////////////// Main Initialization Calls ////////////////////

            if (not PeachCore::GameManager::get_single().InitializePeachEngine(fp_RootPath, f_HostFxrPath, PeachCore::RendererType::Vulkan))
            {

                return false;
            }
            else if (not SetupInternalManagers(f_LogDir))
            {

                return false;
            }

            //else if (not InitializeQueues())
            //{

            //    return false;
            //}

            //////////////////// Dotnet Testing not Real Production Code ////////////////////

            //Serializer f_Serializer;

            //f_Serializer.ToJSON(pm_DotnetContext.RuntimeConfigs, "PeachGame.runtimeconfig", fp_RootPath + "/local_tests", main_logger.get());

            //DotnetUtils::GenerateDefaultScript("FirstGeneratedScript", "Sprite2D", fp_RootPath + "/local_tests", main_logger.get());

            /*DotnetUtils::GenerateProjectFiles(pm_DotnetConfiguration, "PeachGame", fp_RootPath + "/local_tests", fp_RootPath + "res/script_runtimes/win64/dotnet/PeachScriptCore.dll", "", main_editor_logger.get());
            DotnetUtils::BuildDotnetProject(pm_DotnetConfiguration.SolutionPath, main_editor_logger.get());*/

            //////////////////// Success! ////////////////////

            return true;
        }

        ////////////////////////////////////////////////
        // Project File Handling Methods
        ////////////////////////////////////////////////

        bool
            CreatePeachProjectFile //project files are just json files with a different extension name uwu
            (
                const string& fp_ProjectName, 
                const string& fp_TargetDirectory
            ) 
        {

            return true;
        }

        bool
            LoadPeachProjectFile()
        {

            return true;
        }

        bool
            EditProjectFile()
        {

            return true;
        }

        // Function to list all files recursively
        unordered_map<string, filesystem::file_time_type>
            GetCurrentDirectoryState
            (
                const filesystem::path& fp_Directory
            )
        {
            unordered_map<string, filesystem::file_time_type> f_Files;

            try
            {
                for (const auto& _entry : filesystem::recursive_directory_iterator(fp_Directory))
                {
                    if (filesystem::is_regular_file(_entry.status()) or filesystem::is_directory(_entry.status()))
                    {
                        f_Files[_entry.path().string()] = filesystem::last_write_time(_entry);
                    }
                }
            }
            catch (const filesystem::filesystem_error& e)
            {
                main_editor_logger->LogAndPrint("LogAndPrint while checking current directory state: " + static_cast<string>(e.what()), "main", PeachCore::LogManager::LogLevel::Error);
            }

            return f_Files;
        }

    //////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////
    private:

        ////////////////////////////////////////////////
        // Setup Communication Queues
        ////////////////////////////////////////////////

        bool
            InitializeQueues()

        {
            auto editor_renderer = &PeachEditor::PeachEditorRenderingManager::get_single();

            //used for pushing update commands to the Render Thread
            //Initialize methods, so RenderingManager issues one and only one copy of the commandqueue sharedptr for the main thread to use judiciously
            pm_PeachEditorRenderingManagersCommandQueue = editor_renderer->InitializeQueues(); //lifetime is tied to renderingmanager so fuck u main thread, if renderingmanager says commandqueue is out, command queue is out
            //pm_PeachEditorDrawableResourceLoadingQueue = PeachEditor::PeachEditorResourceLoadingManager::PeachEditorResourceLoader().GetDrawableResourceLoadingQueue(); //used to push load commands that are destined for RenderingManager

            // ObjectID : SceneTreeItem : Associated Update Package, used for updating all relevant data at the same time
            //map<string, PeachNode, UpdateActiveDrawableData> m_MapOfAllCurrentlyActivePeachNodes;
            //map<string, PeachNode, UpdateActiveDrawableData> m_MapOfAllPeachNodesQueuedForRemoval;
            // Function to compare two fs states, returns true if file_system_1 == file_system_2, returns false otherwise

            return true;
        }

        ////////////////////////////////////////////////
        // Directory Detection Functions
        ////////////////////////////////////////////////

        bool
            CompareStates
            (
                const unordered_map<string, filesystem::file_time_type>& fp_OldState,
                const unordered_map <string, filesystem::file_time_type>& fp_NewState
            )
        {
            for (const auto& _file : fp_NewState)
            {
                auto it = fp_OldState.find(_file.first);

                if (it == fp_OldState.end())
                {
                    main_editor_logger->LogAndPrint("New file found in working directory: " + _file.first, "main", PeachCore::LogManager::LogLevel::Debug);
                    return false;
                }
                else if (it->second != _file.second)
                {
                    main_editor_logger->LogAndPrint("Modified file found in working directory: " + _file.first, "main", PeachCore::LogManager::LogLevel::Trace);
                    return false;
                }
            }

            for (const auto& _file : fp_OldState)
            {
                if (fp_NewState.find(_file.first) == fp_NewState.end())
                {
                    main_editor_logger->LogAndPrint("Deleted file from working directory: " + _file.first, "main", PeachCore::LogManager::LogLevel::Debug);
                    return false;
                }
            }

            return true;
        }

        void
            CheckAndUpdateFileSystem() //XXX: this function seems kinda sus idk if it works as i want it too lmfao
        {
            auto f_CurrentPath = filesystem::current_path(); //idfk
            auto f_InitialPathState = GetCurrentDirectoryState(f_CurrentPath);

            auto f_NewState = GetCurrentDirectoryState(f_CurrentPath);

            if (f_NewState != f_InitialPathState)
            {
                CompareStates(f_InitialPathState, f_NewState);
                f_InitialPathState = move(f_NewState);
            }
        }
    };
}