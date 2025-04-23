#pragma once

#include "../../Peach-Engine/GameManager.h"
#include "PeachEditorRenderingManager.h"
#include "../Editor/PeachProject.h"

using namespace std;

namespace PeachEditor{

    class PeachEditorManager
    {
    //////////////////////////////////////////////
    // Destructor
    //////////////////////////////////////////////
    public:
        ~PeachEditorManager() {}

    //////////////////////////////////////////////
    // Private Constructor
    //////////////////////////////////////////////
    private:
        PeachEditorManager() = default;

        PeachEditorManager(const PeachEditorManager&) = delete;
        PeachEditorManager& operator=(const PeachEditorManager&) = delete;

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

    //////////////////////////////////////////////
    // Public Members
    //////////////////////////////////////////////
    public:
        static PeachEditorManager& PeachEditor() 
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
            auto peach_engine = &PeachEngine::GameManager::PeachEngine();
            auto editor_renderer = &PeachEditor::PeachEditorRenderingManager::PeachEditorRenderer();

            bool mf_IsEditorOpen = true;

            while (mf_IsEditorOpen)
            {
                this_thread::sleep_for(chrono::milliseconds(16)); //60 fps oh i just realized the fps flickers by 1 because the floating point conversion isnt exact
                editor_renderer->RenderFrame(&mf_IsEditorOpen);
            }

            editor_renderer->Shutdown();
            peach_engine->ShutdownPeachEngine();

            main_editor_logger->LogAndPrint("Exit Success!", "Peach-E", PeachCore::LogManager::LogLevel::Debug);
        }

        bool
            SetupInternalLogManagers(const string& fp_RootPath)
        {
            const string f_LogDir = fp_RootPath + "/logs";

            pm_PeachEditorConsole = make_shared<PeachCore::Console>();

            main_editor_logger = make_unique<PeachCore::LogManager>();

            if (not main_editor_logger->Initialize("main_thread", f_LogDir, "PeachEditorManager", pm_PeachEditorConsole))
            {
                PeachCore::PrintError("Initialization error: Was not able to initialize PeachEditorManager's main logger");
                return false;
            }

            main_editor_logger->LogAndPrint("Main editor logger successfully initialized", "PeachEditorManager", PeachCore::LogManager::LogLevel::Debug);

            //probably should have better error handling for the loggers, especially
            //main_editor_logger->Initialize("..\\logs", f_PeachConsole);
            //InternalLogManager::InternalAudioLogger().Initialize("..\\logs", "audio_thread", f_PeachConsole);
            //if(not PeachEditorRenderingManager::PeachEditorRenderer().Initialize(f_LogDir, pm_PeachEditorConsole))
            //{
            //    main_editor_logger->LogAndPrint("Initialization error: PeachEditorRenderer failed to initialize properly, exiting program execution immediately", "PeachEditorManager", PeachCore::LogManager::LogLevel::Fatal);
            //    return false;
            //}

            if(not PeachEditorResourceLoadingManager::PeachEditorResourceLoader().InitializeLogger(f_LogDir, pm_PeachEditorConsole))
            {
                main_editor_logger->LogAndPrint("Initialization error: PeachEditorResourceLoader failed to initialize properly, exiting program execution immediately", "PeachEditorManager", PeachCore::LogManager::LogLevel::Fatal);
                return false;
            }

            //main_editor_logger->LogAndPrint("InternalMainLogger successfully initialized", "Peach-E", "debug");
            //InternalLogManager::InternalAudioLogger().LogAndPrint("InternalAudioLogger successfully initialized", "Peach-E", "debug");
            //InternalLogManager::InternalResourceLoadingLogger().LogAndPrint("InternalResourceLoadingLogger successfully initialized", "Peach-E", "debug");

            return true;
        }

        bool
            InitializePeachEditor(const string& fp_RootPath) //XXX: idk this method seems kinda weird idk how im gonna manage error codes but w/e thats for future me to handle UwU
        {
            auto peach_engine = &PeachEngine::GameManager::PeachEngine();

            if (not peach_engine->InitializePeachEngine(fp_RootPath, PeachCore::RendererType::Vulkan))
            {

                return false;
            }
            else if (not SetupInternalLogManagers(fp_RootPath))
            {

                return false;
            }
            //else if (not InitializeQueues())
            //{

            //    return false;
            //}

            PeachCore::Serializer Serializer;

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
            auto editor_renderer = &PeachEditor::PeachEditorRenderingManager::PeachEditorRenderer();

            //used for pushing update commands to the Render Thread
            //Initialize methods, so RenderingManager issues one and only one copy of the commandqueue sharedptr for the main thread to use judiciously
            pm_PeachEditorRenderingManagersCommandQueue = editor_renderer->InitializeQueues(); //lifetime is tied to renderingmanager so fuck u main thread, if renderingmanager says commandqueue is out, command queue is out
            pm_PeachEditorDrawableResourceLoadingQueue = PeachEditor::PeachEditorResourceLoadingManager::PeachEditorResourceLoader().GetDrawableResourceLoadingQueue(); //used to push load commands that are destined for RenderingManager

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