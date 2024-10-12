#pragma once

#include "InternalLogManager.h"
#include "../Peach-Engine/PeachEngineManager.h"

using namespace std;

namespace fs = filesystem;

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

    //////////////////////////////////////////////
    // Public Members
    //////////////////////////////////////////////
    public:
        static PeachEditorManager& PeachEditor() {
            static PeachEditorManager peach_editor;
            return peach_editor;
        }

        static atomic<bool> m_IsRunning;

    //////////////////////////////////////////////
    // Public Methods
    //////////////////////////////////////////////

    public:
        static void
            SetupInternalLogManagers()
        {
            shared_ptr<PeachConsole> f_PeachConsole = make_shared<PeachConsole>();

            //probably should have better error handling for the loggers, especially
            InternalLogManager::InternalMainLogger().Initialize("..\\logs", "main_thread", f_PeachConsole);
            InternalLogManager::InternalAudioLogger().Initialize("..\\logs", "audio_thread", f_PeachConsole);
            InternalLogManager::InternalRenderingLogger().Initialize("..\\logs", "render_thread", f_PeachConsole);
            InternalLogManager::InternalResourceLoadingLogger().Initialize("..\\logs", "resource_thread", f_PeachConsole);

            InternalLogManager::InternalMainLogger().LogAndPrint("InternalMainLogger successfully initialized", "Peach-E", "debug");
            InternalLogManager::InternalAudioLogger().LogAndPrint("InternalAudioLogger successfully initialized", "Peach-E", "debug");
            InternalLogManager::InternalRenderingLogger().LogAndPrint("InternalRenderingLogger successfully initialized", "Peach-E", "debug");
            InternalLogManager::InternalResourceLoadingLogger().LogAndPrint("InternalResourceLoadingLogger successfully initialized", "Peach-E", "debug");
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
                InternalLogManager::InternalMainLogger().LogAndPrint("LogAndPrint while checking current directory state: " + static_cast<string>(e.what()), "main", "error");
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
            auto main_logger = &InternalLogManager::InternalMainLogger();

            for (const auto& _file : fp_NewState)
            {
                auto it = fp_OldState.find(_file.first);

                if (it == fp_OldState.end())
                {
                    main_logger->LogAndPrint("New file found in working directory: " + _file.first, "main", "debug");
                    return false;
                }
                else if (it->second != _file.second)
                {
                    main_logger->LogAndPrint("Modified file found in working directory: " + _file.first, "main", "trace");
                    return false;
                }
            }

            for (const auto& _file : fp_OldState)
            {
                if (fp_NewState.find(_file.first) == fp_NewState.end())
                {
                    main_logger->LogAndPrint("Deleted file from working directory: " + _file.first, "main", "debug");
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