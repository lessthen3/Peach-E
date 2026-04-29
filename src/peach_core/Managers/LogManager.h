/*******************************************************************
 *                        Peach-E v0.0.1
 *         Created by Ranyodh Singh Mandur - 🍑 2024-2026
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#pragma once

///PeachCore
#include "Utils/Logger.h"

///STL
#include <memory>

namespace PeachCore{

    struct LogManager
    {
    private:
        std::vector<Logger*> pm_GlobalLoggerRegistry;

        // Called from Logger ctor; Logger is a friend so it can call this.
        void
            RegisterLogger(Logger* fp_Logger)
        {
            std::lock_guard<std::mutex> f_Lock(s_RegistryMutex);
            s_Registry.push_back(fp_Logger);
        }

        void
            UnregisterLogger(Logger* fp_Logger) noexcept
        {
            std::lock_guard<std::mutex> f_Lock(s_RegistryMutex);
            auto f_It = std::find(s_Registry.begin(), s_Registry.end(), fp_Logger);
            if (f_It != s_Registry.end())
            {
                s_Registry.erase(f_It);
            }
        }

    friend class Logger;
    
    public:

        LogManager() = default;

        static LogManager& get_single()
        {
            static LogManager log_manager;
            return log_manager;
        }

        LogManager(const LogManager&) = delete;
        LogManager& operator=(const LogManager&) = delete;
        LogManager(LogManager&&) = delete;
        LogManager& operator=(LogManager&&) = delete;

    public:

        [[nodiscard]] static unique_ptr<Logger>
            CreateUniqueLogger
            (
                const string& fp_DesiredLoggerName,
                const PEACH_LOGGER_FLAGS fp_Flags,
                const string& fp_DesiredOutputDirectory = ""
            )
        {
            unique_ptr<Logger> f_CreatedLogger(new Logger()); //this is dumb but std doesn't like my private constructor uwu!

            if (not f_CreatedLogger->Initialize(fp_DesiredLoggerName, fp_DesiredOutputDirectory, fp_Flags))
            {
                PEACH_PRINT_ERROR_FMT("Unable to initialize logger named: {}", fp_DesiredLoggerName);
                return nullptr;
            }

            return f_CreatedLogger;
        }

        [[nodiscard]] static shared_ptr<Logger>
            CreateSharedLogger
            (
                const string& fp_DesiredLoggerName,
                const PEACH_LOGGER_FLAGS fp_Flags,
                const string& fp_DesiredOutputDirectory = ""
            )
        {
            shared_ptr<Logger> f_CreatedLogger(new Logger()); //this is dumb but std doesn't like my private constructor uwu!

            if (not f_CreatedLogger->Initialize(fp_DesiredLoggerName, fp_DesiredOutputDirectory, fp_Flags))
            {
                PEACH_PRINT_ERROR_FMT("Unable to initialize logger named: {}", fp_DesiredLoggerName);
                return nullptr;
            }

            return f_CreatedLogger;
        }
    };
}