/*******************************************************************
 *                                        Peach Editor v0.0.7
 *                           Created by Ranyodh Mandur - � 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                 Peach Editor is an open source editor for Peach-E
********************************************************************/
#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>
#include <filesystem>
#include <mutex>

#include "../Editor/PeachConsole.h"

using namespace std;

namespace PeachEditor {

    class InternalLogManager 
    {
    private:
        ~InternalLogManager() = default;

    private:
        bool pm_IsInitialized = false; //set to false intially, and will be set to true once intialized to prevent more than one initialization

        shared_ptr<spdlog::logger> logger;
        shared_ptr<PeachConsole> pm_EditorConsole = nullptr;

    public:
        string m_LoggerName;

    private:
        InternalLogManager() = default;

        InternalLogManager(const InternalLogManager&) = delete;
        InternalLogManager& operator=(const InternalLogManager&) = delete;

    public:
        static InternalLogManager& 
            InternalRenderingLogger() 
        {
            static InternalLogManager internal_rendering_logger;
            return internal_rendering_logger;
        }

        static InternalLogManager& 
            InternalMainLogger() 
        {
            static InternalLogManager internal_main_logger;
            return internal_main_logger;
        }

        static InternalLogManager& 
            InternalAudioLogger() 
        {
            static InternalLogManager internal_audio_logger;
            return internal_audio_logger;
        }

        static InternalLogManager& 
            InternalResourceLoadingLogger() 
        {
            static InternalLogManager internal_resource_loader_logger;
            return internal_resource_loader_logger;
        }

        void 
            Initialize
            (
                const string& logDirectory, 
                const string& fp_LoggerName,
                const shared_ptr<PeachConsole> fp_PeachConsole
            );

        void 
            Trace(const string& message, const string& className);
        void 
            Debug(const string& message, const string& className);
        void 
            Info(const string& message, const string& className);
        void 
            Warn(const string& message, const string& className);
        void 
            Error(const string& message, const string& className);
        void 
            Fatal(const string& message, const string& className);


        void 
            CreateLogFiles(const string& logDirectory)
                const;

        shared_ptr<PeachConsole>
            GetConsole()
        {
            return pm_EditorConsole;
        }
    };
}
