#pragma once

#include "../Peach-Engine/GameManager.h"
#include "PeachEditorRenderingManager.h"

using namespace std;

namespace fs = filesystem;
namespace PC = PeachCore;

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
        shared_ptr<PC::Console> pm_PeachEditorConsole = nullptr;
    public: //IDK PUBLIC FOR NOW CAUSE OF MAIN.CPP FAQQ im tried man i just wanna compile again
        shared_ptr<PC::LogManager> main_editor_logger = nullptr;

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

            main_editor_logger->LogAndPrint("Exit Success!", "Peach-E", PeachCore::LogManager::LogLevel::Debug, "main_thread");
        }

        bool
            SetupInternalLogManagers(const string& fp_RootPath)
        {
            const string f_LogDir = fp_RootPath + "/logs";

            pm_PeachEditorConsole = make_shared<PC::Console>();

            main_editor_logger = make_unique<PC::LogManager>();

            if (not main_editor_logger->Initialize(f_LogDir, "PeachEditorManager", pm_PeachEditorConsole))
            {
                PeachCore::PrintError("Initialization error: Was not able to initialize PeachEditorManager's main logger");
                return false;
            }

            main_editor_logger->LogAndPrint("Main editor logger successfully initialized", "PeachEditorManager", PeachCore::LogManager::LogLevel::Debug, "main_thread");

            //probably should have better error handling for the loggers, especially
            //main_editor_logger->Initialize("..\\logs", "main_thread", f_PeachConsole);
            //InternalLogManager::InternalAudioLogger().Initialize("..\\logs", "audio_thread", f_PeachConsole);
            if(not PeachEditorRenderingManager::PeachEditorRenderer().Initialize(f_LogDir, pm_PeachEditorConsole))
            {
                main_editor_logger->LogAndPrint("Initialization error: PeachEditorRenderer failed to initialize properly, exiting program execution immediately", "PeachEditorManager", PeachCore::LogManager::LogLevel::Fatal, "main_thread");
                return false;
            }

            if(not PeachEditorResourceLoadingManager::PeachEditorResourceLoader().InitializeLogger(f_LogDir, pm_PeachEditorConsole))
            {
                main_editor_logger->LogAndPrint("Initialization error: PeachEditorResourceLoader failed to initialize properly, exiting program execution immediately", "PeachEditorManager", PeachCore::LogManager::LogLevel::Fatal, "main_thread");
                return false;
            }

            //main_editor_logger->LogAndPrint("InternalMainLogger successfully initialized", "Peach-E", "debug");
            //InternalLogManager::InternalAudioLogger().LogAndPrint("InternalAudioLogger successfully initialized", "Peach-E", "debug");
            //InternalLogManager::InternalResourceLoadingLogger().LogAndPrint("InternalResourceLoadingLogger successfully initialized", "Peach-E", "debug");

            return true;
        }

        bool
            InitializePeachEditor(const string& fp_RootPath)
        {
            if (not SetupInternalLogManagers(fp_RootPath))
            {

                return false;
            }

            return true;
        }

        ////////////////////////////////////////////////
        // Project File Handling Methods
        ////////////////////////////////////////////////

        bool
            CreatePeachProjectFile() //this is here for adjusting the JSON configs from the Peach Editor
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
        unordered_map<string, fs::file_time_type>
            GetCurrentDirectoryState
            (
                const fs::path& fp_Directory
            )
        {
            unordered_map<string, fs::file_time_type> f_Files;

            try
            {
                for (const auto& _entry : fs::recursive_directory_iterator(fp_Directory))
                {
                    if (fs::is_regular_file(_entry.status()) || fs::is_directory(_entry.status()))
                    {
                        f_Files[_entry.path().string()] = fs::last_write_time(_entry);
                    }
                }
            }
            catch (const fs::filesystem_error& e)
            {
                main_editor_logger->LogAndPrint("LogAndPrint while checking current directory state: " + static_cast<string>(e.what()), "main", PeachCore::LogManager::LogLevel::Error, "main_thread");
            }

            return f_Files;
        }

    //////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////
    private:
        // Function to compare two fs states, returns true if file_system_1 == file_system_2, returns false otherwise
        bool
            CompareStates
            (
                const unordered_map<string, fs::file_time_type>& fp_OldState,
                const unordered_map <string, fs::file_time_type>& fp_NewState
            )
        {
            for (const auto& _file : fp_NewState)
            {
                auto it = fp_OldState.find(_file.first);

                if (it == fp_OldState.end())
                {
                    main_editor_logger->LogAndPrint("New file found in working directory: " + _file.first, "main", PeachCore::LogManager::LogLevel::Debug, "main_thread");
                    return false;
                }
                else if (it->second != _file.second)
                {
                    main_editor_logger->LogAndPrint("Modified file found in working directory: " + _file.first, "main", PeachCore::LogManager::LogLevel::Trace, "main_thread");
                    return false;
                }
            }

            for (const auto& _file : fp_OldState)
            {
                if (fp_NewState.find(_file.first) == fp_NewState.end())
                {
                    main_editor_logger->LogAndPrint("Deleted file from working directory: " + _file.first, "main", PeachCore::LogManager::LogLevel::Debug, "main_thread");
                    return false;
                }
            }

            return true;
        }

        void
            CheckAndUpdateFileSystem()
        {
            auto f_CurrentPath = fs::current_path(); //idfk
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