#include "../../include/Managers/LogManager.h"
#include <fstream>
#include <iostream>

namespace PeachCore {

    void LogManager::Initialize(const std::string& logDirectory, const std::string& fp_LoggerName = "Logger") {

        if (hasBeenInitialized) //stops accidental reinitialization of logmanager
        {
            std::cout << "LogManager has already been initialized, LogManager is only allowed to initialize once per run\n";
            return;
        }
        else if (fp_LoggerName == "") //prevents any directory nonsense
        {
            std::cout << "Stop playin and input a logger name that isnt an empty string, because it's going to break the directory structure otherwise" << "\n";
            return;
        }
        m_LoggerName = fp_LoggerName;

        // Ensure log directory exists
        if (!std::filesystem::exists(logDirectory)) 
        {
            try 
            {
                std::filesystem::create_directories(logDirectory);
            }
            catch (const std::exception& ex)
            {
                std::cerr << "An error occurred: " << ex.what() << std::endl;
                return;
            }
            
        }
        // Create log files if they don't exist
        CreateLogFiles(logDirectory);
        
        // Define sinks
        std::vector<spdlog::sink_ptr> sinks;

        // Add console sink
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

        // Add file sinks for every debug level plus an extra sink that contains all log levels in one file
        sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logDirectory + "/" + fp_LoggerName + "/trace.log", true));
        sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logDirectory + "/" + fp_LoggerName + "/debug.log", true));
        sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logDirectory + "/" + fp_LoggerName + "/info.log", true));
        sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logDirectory + "/" + fp_LoggerName + "/warn.log", true));
        sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logDirectory + "/" + fp_LoggerName + "/error.log", true));
        sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logDirectory + "/" + fp_LoggerName + "/fatal.log", true));
        sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logDirectory + "/" + fp_LoggerName + "/all-logs.log", true));

        // Create and register the logger
        logger = std::make_shared<spdlog::logger>(fp_LoggerName, sinks.begin(), sinks.end());
        logger->set_level(spdlog::level::trace);  // Set the logger's level to trace
        logger->flush_on(spdlog::level::trace);   // Flush the logger on every trace log message

        spdlog::register_logger(logger);  // Register the logger with spdlog
       
        hasBeenInitialized = true; //logger should be fully initialized successfully
    }

    void LogManager::Trace(const std::string& message, const std::string& className) {
        logger->trace("[{}] {}", className, message);
    }

    void LogManager::Debug(const std::string& message, const std::string& className) {
        logger->debug("[{}] {}", className, message);
    }

    void LogManager::Info(const std::string& message, const std::string& className) {
        logger->info("[{}] {}", className, message);
    }

    void LogManager::Warn(const std::string& message, const std::string& className) {
        logger->warn("[{}] {}", className, message);
    }

    void LogManager::Error(const std::string& message, const std::string& className) {
        logger->error("[{}] {}", className, message);
    }

    void LogManager::Fatal(const std::string& message, const std::string& className) {
        logger->critical("[{}] {}", className, message);
    }

    void LogManager::CreateLogFiles(const std::string& logDirectory) {
        std::vector<std::string> logFiles = {
            "trace.log",
            "debug.log",
            "info.log",
            "warn.log",
            "error.log",
            "fatal.log",
            "all-logs.log"
        };

        for (const auto& file : logFiles) {
            std::string filePath = logDirectory + "/" + m_LoggerName + "/" + file;
            if (!std::filesystem::exists(filePath)) {
                std::ofstream ofs(filePath); // This will create the file if it doesn't exist
            }
        }
    }
}