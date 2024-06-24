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

    void init(const std::string& logDirectory);

    void trace(const std::string& message, const std::string& className);
    void debug(const std::string& message, const std::string& className);
    void info(const std::string& message, const std::string& className);
    void warn(const std::string& message, const std::string& className);
    void error(const std::string& message, const std::string& className);
    void fatal(const std::string& message, const std::string& className);

private:
    LogManager() = default;
    ~LogManager() = default;

    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;

    std::shared_ptr<spdlog::logger> logger;
    std::mutex logMutex;

    void createLogFiles(const std::string& logDirectory);
};
