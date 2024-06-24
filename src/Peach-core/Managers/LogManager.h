#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <filesystem>
#include <mutex>
#include <string>

class LogManager {
public:
    static LogManager& Logger() {
        static LogManager instance;
        return instance;
    }

    void Init(const std::string& logDirectory);

    void Trace(const std::string& message, const std::string& className);
    void Debug(const std::string& message, const std::string& className);
    void Info(const std::string& message, const std::string& className);
    void Warn(const std::string& message, const std::string& className);
    void Error(const std::string& message, const std::string& className);
    void Fatal(const std::string& message, const std::string& className);

private:
    LogManager() = default;
    ~LogManager() = default;

    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;

    std::shared_ptr<spdlog::logger> logger;
    std::mutex logMutex;

    void CreateLogFiles(const std::string& logDirectory);
};
