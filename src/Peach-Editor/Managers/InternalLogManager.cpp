#include "../../include/Peach-Editor/Managers/InternalLogManager.h"
#include <fstream>
#include <iostream>

using namespace std;

namespace PeachEditor {

    void 
        InternalLogManager::Initialize
        (
            const string& logDirectory, 
            const string& fp_LoggerName = "Logger",
            const shared_ptr<PeachConsole> fp_PeachConsole = nullptr
        ) 
    {

        if (pm_IsInitialized) //stops accidental reinitialization of logmanager
        {
            cout << "InternalLogManager has already been initialized, InternalLogManager is only allowed to initialize once per run\n";
            return;
        }
        else if (fp_LoggerName == "") //prevents any directory nonsense
        {
            cout << "Stop playin and input a logger name that isnt an empty string, because it's going to break the directory structure otherwise" << "\n";
            return;
        }
        m_LoggerName = fp_LoggerName;

        // Ensure log directory exists
        if (!filesystem::exists(logDirectory))
        {
            try
            {
                filesystem::create_directories(logDirectory);
            }
            catch (const exception& ex)
            {
                cerr << "An error occurred: " << ex.what() << endl;
                return;
            }

        }
        // Create log files if they don't exist
        CreateLogFiles(logDirectory);

        // Define sinks
        vector<spdlog::sink_ptr> sinks;

        // Add console sink
        sinks.push_back(make_shared<spdlog::sinks::stdout_color_sink_mt>());

        // Add file sinks for every debug level plus an extra sink that contains all log levels in one file
        sinks.push_back(make_shared<spdlog::sinks::basic_file_sink_mt>(logDirectory + "/" + fp_LoggerName + "/trace.log", true));
        sinks.push_back(make_shared<spdlog::sinks::basic_file_sink_mt>(logDirectory + "/" + fp_LoggerName + "/debug.log", true));
        sinks.push_back(make_shared<spdlog::sinks::basic_file_sink_mt>(logDirectory + "/" + fp_LoggerName + "/info.log", true));
        sinks.push_back(make_shared<spdlog::sinks::basic_file_sink_mt>(logDirectory + "/" + fp_LoggerName + "/warn.log", true));
        sinks.push_back(make_shared<spdlog::sinks::basic_file_sink_mt>(logDirectory + "/" + fp_LoggerName + "/error.log", true));
        sinks.push_back(make_shared<spdlog::sinks::basic_file_sink_mt>(logDirectory + "/" + fp_LoggerName + "/fatal.log", true));
        sinks.push_back(make_shared<spdlog::sinks::basic_file_sink_mt>(logDirectory + "/" + fp_LoggerName + "/all-logs.log", true));

        // Create and register the logger
        logger = make_shared<spdlog::logger>(fp_LoggerName, sinks.begin(), sinks.end());
        logger->set_level(spdlog::level::trace);  // Set the logger's level to trace
        logger->flush_on(spdlog::level::trace);   // Flush the logger on every trace log message for debugging and early development

        spdlog::register_logger(logger);  // Register the logger with spdlog

        pm_EditorConsole = fp_PeachConsole;

        pm_IsInitialized = true; //logger should be fully initialized successfully
    }

    void 
        InternalLogManager::Trace(const string& fp_Message, const string& fp_Sender) 
    {
        logger->trace("[{}] {}", fp_Sender, fp_Message);
        pm_EditorConsole->AddLog("[" + m_LoggerName + "]: " + fp_Message + "\n");
    }

    void 
        InternalLogManager::Debug(const string& fp_Message, const string& fp_Sender) 
    {
        logger->debug("[{}] {}", fp_Sender, fp_Message);
        pm_EditorConsole->AddLog("[" + m_LoggerName + "]: " + fp_Message + "\n");
    }

    void 
        InternalLogManager::Info(const string& fp_Message, const string& fp_Sender) 
    {
        logger->info("[{}] {}", fp_Sender, fp_Message);
        pm_EditorConsole->AddLog("[" + m_LoggerName + "]: " + fp_Message + "\n");
    }

    void 
        InternalLogManager::Warn(const string& fp_Message, const string& fp_Sender) 
    {
        logger->warn("[{}] {}", fp_Sender, fp_Message);
        pm_EditorConsole->AddLog("[" + m_LoggerName + "]: " + fp_Message + "\n");
    }

    void 
        InternalLogManager::Error(const string& fp_Message, const string& fp_Sender) 
    {
        logger->error("[{}] {}", fp_Sender, fp_Message);
        pm_EditorConsole->AddLog("[" + m_LoggerName + "]: " + fp_Message + "\n");
    }

    void 
        InternalLogManager::Fatal(const string& fp_Message, const string& fp_Sender)
    {
        logger->critical("[{}] {}", fp_Sender, fp_Message);
        pm_EditorConsole->AddLog("[" + m_LoggerName + "]: " + fp_Message + "\n");
    }

    void 
        InternalLogManager::CreateLogFiles
        (
            const string& fp_DesiredLogDirectory
        ) 
        const
    {
        vector<string> 
            f_LogFiles = 
        {
            "trace.log",
            "debug.log",
            "info.log",
            "warn.log",
            "error.log",
            "fatal.log",
            "all-logs.log"
        };

        for (const auto& _file : f_LogFiles)
        {
            string f_FilePath = fp_DesiredLogDirectory + "/" + m_LoggerName + "/" + _file;

            if (!filesystem::exists(f_FilePath))
            {
                ofstream ofs(f_FilePath); // This will create the file if it doesn't exist
            }
        }
    }
}