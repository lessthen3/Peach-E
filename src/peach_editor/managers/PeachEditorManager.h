/*******************************************************************
 *                     Peach Editor v0.0.1
 *         Created by Ranyodh Singh Mandur - 🍑 2024-2026
 *
 *             Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *             https://opensource.org/licenses/MIT
 *
 *      Peach Editor is a free open source editor for Peach-E
********************************************************************/
#pragma once

///PeachCore owo
#include <managers/GameManager.h>

///PeachEditor uwu 
#include "editor/LangUtils.h"

namespace PeachEditor {

    using namespace std;

    class PeachEditorManager
    {
    public:
        ~PeachEditorManager() = default;

        //////////////////////////////////////////////
        // Private Constructor & Destructor
        //////////////////////////////////////////////
    private:
        PeachEditorManager() = default;

        PeachEditorManager(const PeachEditorManager&) = delete;
        PeachEditorManager& operator=(const PeachEditorManager&) = delete;

        PeachEditorManager(PeachEditorManager&&) = delete;
        PeachEditorManager& operator=(PeachEditorManager&&) = delete;

        //////////////////////////////////////////////
        // Private Members
        //////////////////////////////////////////////
    private:
        unique_ptr<PeachCore::Logger> main_editor_logger = nullptr;

        Dotnet::Configs pm_DotnetConfiguration;

        //////////////////////////////////////////////
        // Public Members
        //////////////////////////////////////////////
    public:
        static PeachEditorManager& get_single()
        {
            static PeachEditorManager peach_editor;
            return peach_editor;
        }

        atomic<bool> m_IsRunning;

        //////////////////////////////////////////////
        // Public Methods
        //////////////////////////////////////////////

    public:
        ////////////////////////////////////////////////
        // Start Main Loop
        ////////////////////////////////////////////////
        void
            StartPeachEditorMainLoop() //this is where 
        {
            auto& engine_manager = PeachCore::GameManager::get_single();

            engine_manager.StartMainGameLoop();
            engine_manager.ShutdownPeachEngine();

            PEACH_LOG_DEBUG(main_editor_logger, "Exit Success!", "Peach-E");
        }

        bool
            InitializePeachEditor(const string& fp_RootPath) //XXX: idk this method seems kinda weird idk how im gonna manage error codes but w/e thats for future me to handle UwU
        {
            //////////////////// Create Main Thread Logger ////////////////////

            const string f_LogDir = fp_RootPath + "/logs";

            main_editor_logger = PeachCore::LogManager::get_single().CreateUniqueLogger("PeachEditorManager", PEACH_LOGGER_DEFAULT_FLAGS, f_LogDir);

            if (not main_editor_logger)
            {
                PEACH_PRINT_ERROR("Initialization error: Was not able to initialize PeachEditorManager's main logger");
                return false;
            }

            main_editor_logger->Info("Main editor logger successfully initialized", "PeachEditorManager");

#ifdef PEACH_PLATFORM_APPLE
            auto f_RenderingBackend = PeachCore::RendererType::Metal; //TESTING: apple only supports metal but the other plats need a default renderer and meta data to save the backend set by usr
#else
            auto f_RenderingBackend = PeachCore::RendererType::Vulkan;
#endif

            //////////////////// Main Initialization Calls //////////////////// 
            // //NEEDA: figure out a better way to handle dotnet projects, maybe feed a string like "NUHUH" to signal the InitializePeachEngine call that this aint a dotnet game

            if
                (
                    not PeachCore::GameManager::get_single().InitializePeachEngineCustom
                    (
                        fp_RootPath,
                        PeachCore::Subsystem::Render | PeachCore::Subsystem::Physics2D | PeachCore::Subsystem::Network | PeachCore::Subsystem::Audio, 
                        f_RenderingBackend,
                        800, 600
                    )
                )
            {

                return false;
            }

            //////////////////// Success! ////////////////////

            return true;
        }

        //////////////////////////////////////////////
        // Private Methods
        //////////////////////////////////////////////
    private:
        //////////////////// Project File Handling Methods ////////////////////

        bool
            CreatePeachProjectFile //project files are just json files with a different extension name uwu
            (
                const string& fp_ProjectName,
                const string& fp_TargetDirectory
            )
        {
            PEACH_TO_DO_UNUSED(fp_ProjectName);
            PEACH_TO_DO_UNUSED(fp_TargetDirectory);

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

        bool
            GenerateDotnetProject()
        {
            //////////////////// Get Local HostFxr Path and Validate Dotnet Exists TEST_ONLY ////////////////////

            string f_HostFxrPath;

            if (Dotnet::AssertDotnetExists()) //dummy call but should actually make lmfao
            {
                Dotnet::GetHostFxrLocalPath(&f_HostFxrPath, main_editor_logger.get());
            }

            //DotnetUtils::GenerateDefaultScript("FirstGeneratedScript", "Sprite2D", fp_RootPath + "/local_tests", main_logger.get());
            //DotnetUtils::GenerateProjectFiles(pm_DotnetConfiguration, "PeachGame", fp_RootPath + "/local_tests", fp_RootPath + "res/script_runtimes/win64/dotnet/PeachScriptCore.dll", "", main_editor_logger.get());
            //DotnetUtils::BuildDotnetProject(pm_DotnetConfiguration.SolutionPath, main_editor_logger.get());

            return true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };
}