#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <filesystem>
#include <mutex>
#include <string>

namespace PeachCore {

    class LogManager {
    public:
        static LogManager& Logger() {
            static LogManager logger;
            return logger;
        }

        static LogManager& RenderingLogger() {
            static LogManager rendering_logger;
            return rendering_logger;
        }

        static LogManager& MainLogger() {
            static LogManager main_logger;
            return main_logger;
        }

        static LogManager& AudioLogger() {
            static LogManager audio_logger;
            return audio_logger;
        }

        static LogManager& NetworkLogger() {
            static LogManager network_logger;
            return network_logger;
        }

        static LogManager& ResourceLoadingLogger() {
            static LogManager resourceloader_logger;
            return resourceloader_logger;
        }

        void Initialize(const std::string& logDirectory, const std::string& fp_LoggerName);

        void Trace(const std::string& message, const std::string& className);
        void Debug(const std::string& message, const std::string& className);
        void Info(const std::string& message, const std::string& className);
        void Warn(const std::string& message, const std::string& className);
        void Error(const std::string& message, const std::string& className);
        void Fatal(const std::string& message, const std::string& className);

    public:
        std::string m_LoggerName;

    private:
        LogManager() = default;
        ~LogManager() = default;

        LogManager(const LogManager&) = delete;
        LogManager& operator=(const LogManager&) = delete;

        std::shared_ptr<spdlog::logger> logger;

        void CreateLogFiles(const std::string& logDirectory);

    private:
        bool hasBeenInitialized = false; //set to false intially, and will be set to true once intialized to prevent more than one initialization
    };
}