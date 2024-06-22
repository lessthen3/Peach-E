#include "LogManager.h"
#include <fstream>

void LogManager::init(const std::string& logDirectory) {
    std::lock_guard<std::mutex> lock(logMutex);

    // Ensure log directory exists
    if (!std::filesystem::exists(logDirectory)) {
        std::filesystem::create_directories(logDirectory);
    }

    // Create log files if they don't exist
    createLogFiles(logDirectory);

    // Define sinks
    std::vector<spdlog::sink_ptr> sinks;

    // Add console sink
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

    // Add file sinks
    sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logDirectory + "/trace.log", true));
    sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logDirectory + "/debug.log", true));
    sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logDirectory + "/info.log", true));
    sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logDirectory + "/warn.log", true));
    sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logDirectory + "/error.log", true));
    sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logDirectory + "/fatal.log", true));
    sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logDirectory + "/all-logs.log", true));

    // Create and register the logger
    logger = std::make_shared<spdlog::logger>("logger", sinks.begin(), sinks.end());
    logger->set_level(spdlog::level::trace);  // Set the logger's level to trace
    logger->flush_on(spdlog::level::trace);   // Flush the logger on every trace log message

    spdlog::register_logger(logger);  // Register the logger with spdlog
}

void LogManager::trace(const std::string& message, const std::string& className) {
    std::lock_guard<std::mutex> lock(logMutex);
    logger->trace("[{}] {}", className, message);
}

void LogManager::debug(const std::string& message, const std::string& className) {
    std::lock_guard<std::mutex> lock(logMutex);
    logger->debug("[{}] {}", className, message);
}

void LogManager::info(const std::string& message, const std::string& className) {
    std::lock_guard<std::mutex> lock(logMutex);
    logger->info("[{}] {}", className, message);
}

void LogManager::warn(const std::string& message, const std::string& className) {
    std::lock_guard<std::mutex> lock(logMutex);
    logger->warn("[{}] {}", className, message);
}

void LogManager::error(const std::string& message, const std::string& className) {
    std::lock_guard<std::mutex> lock(logMutex);
    logger->error("[{}] {}", className, message);
}

void LogManager::fatal(const std::string& message, const std::string& className) {
    std::lock_guard<std::mutex> lock(logMutex);
    logger->critical("[{}] {}", className, message);
}

void LogManager::createLogFiles(const std::string& logDirectory) {
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
        std::string filePath = logDirectory + "/" + file;
        if (!std::filesystem::exists(filePath)) {
            std::ofstream ofs(filePath); // This will create the file if it doesn't exist
        }
    }
}
