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
#include "Logger.h"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <fmt/format.h>

#include <filesystem>

static constexpr uint32_t FLUSH_EVERY_N_LOGS = 256u;
static constexpr uint32_t MAX_LOG_FILE_SIZE_BYTES = 10u * 1024u * 1024u; // 10 MB

static constexpr uint8_t FLUSH_TRACE_BIT = 1u << 0;
static constexpr uint8_t FLUSH_DEBUG_BIT = 1u << 1;
static constexpr uint8_t FLUSH_INFO_BIT = 1u << 2;
static constexpr uint8_t FLUSH_WARNING_BIT = 1u << 3;
static constexpr uint8_t FLUSH_ERROR_BIT = 1u << 4;
static constexpr uint8_t FLUSH_FATAL_BIT = 1u << 5;

namespace PeachCore
{
    [[nodiscard]] static string //thank you chat-gpt uwu
        GetCurrentTimestamp()
    {
        const auto now = chrono::system_clock::now();
        auto time_t_now = chrono::system_clock::to_time_t(now);

        tm local_time{};

#if defined(_WIN32) || defined(_WIN64) //needa do this since localtime() isnt threadsafe uwu
        localtime_s(&local_time, &time_t_now);
#else
        localtime_r(&time_t_now, &local_time);
#endif

        stringstream f_AssembledTimeString;
        f_AssembledTimeString << put_time(&local_time, "%Y-%m-%d %H:%M:%S");

        const auto since_epoch = now.time_since_epoch();
        const auto milliseconds = chrono::duration_cast<chrono::milliseconds>(since_epoch).count() % 1000;

        f_AssembledTimeString << '.' << setfill('0') << setw(3) << milliseconds;

        return f_AssembledTimeString.str();
    }

    static constexpr uint8_t
        ExtractLevelMask(uint32_t fp_Flags) noexcept
    {
        return static_cast<uint8_t>(fp_Flags & 0xFF);
    }

    static constexpr uint8_t
        ExtractFlushMask(uint32_t fp_Flags) noexcept
    {
        return static_cast<uint8_t>((fp_Flags >> 8) & 0xFF);
    }
}

namespace PeachCore {

    Logger::~Logger() ///XXX: Just copy and pasted the flushalllogs method because they have the assert at the beginning and wont work with premature exit
    {
        for (auto& lv_LogFile : pm_LogFiles)
        {
            if (lv_LogFile.second.is_open())
            {
                lv_LogFile.second.flush();
            }
        }  // Ensure all logs are flushed before destruction

        CloseOpenLogFiles(); //Closes any files that are open to prevent introducing vulnerabilities in privileged environments
    }

    bool
        Logger::UpdateThreadOwner //the owning thread must update and pass off the logger to be considered valid otherwise it wont uwu
        (
            const thread::id& fp_NewThreadID
        )
    {
        if (not AssertThreadAccess("UpdateThreadOwner")) //can't log here since it's only triggered by improper thread usage which will trigger asserthreadacess again
        {
            PRINT_ERROR(fmt::format("Tried to call UpdateThreadOwner from a thread that didn't own logger named: {}", pm_LoggerName));
            return false;
        }

        pm_ThreadOwnerID = fp_NewThreadID;

        return true;
    }

   bool
        Logger::UpdateActiveMask(const uint32_t fp_NewLogMask)
    {
        ////////////////////////////////////////////// Change Active Mask if logging to snapshot buffer only uwu //////////////////////////////////////////////

        if (not pm_LogToFile)
        {
            pm_ActiveLogMask = ExtractLevelMask(fp_NewLogMask);
            return true;
        }

        ////////////////////////////////////////////// flush all logs before making any changes //////////////////////////////////////////////

        if (not FlushAllLogs())
        {
            return false;
        }

        ////////////////////////////////////////////// clear every file //////////////////////////////////////////////

        pm_LogFiles.clear();

        ////////////////////////////////////////////// Reset Mask //////////////////////////////////////////////

        pm_ActiveLogMask = 0;

        ////////////////////////////////////////////// Create Log files based off of Current active mask uwu //////////////////////////////////////////////

        static const unordered_map<uint8_t, const string> f_LogLevels = //this is fine being static since its not mutable so reading from multiple threads is kosher
        {
            {PEACH_TRACE_LOG, "trace.log"},
            {PEACH_DEBUG_LOG, "debug.log"},
            {PEACH_INFO_LOG, "info.log"},
            {PEACH_WARNING_LOG, "warning.log"},
            {PEACH_ERROR_LOG, "error.log"},
            {PEACH_FATAL_LOG, "fatal.log"}
        };

        for (const auto& [lv_LogEnum, lv_LogStringName] : f_LogLevels)
        {
            if (fp_NewLogMask & lv_LogEnum)
            {
                if (not CreateLogFile(pm_CurrentWorkingDirectory, lv_LogStringName))
                {
                    PRINT_ERROR("Failed to create log file named: " + lv_LogStringName);
                    return false;
                }

                pm_ActiveLogMask |= static_cast<uint8_t>(lv_LogEnum);
            }
        }

        ////////////////////////////////////////////// Success! //////////////////////////////////////////////

        return true;
    }

    //////////////////// Flush All Logs ////////////////////

    bool
        Logger::FlushAllLogs()
    {
        if (not AssertThreadAccess("FlushAllLogs"))
        {
            return false;
        }

        for (auto& lv_LogFile : pm_LogFiles)
        {
            if (lv_LogFile.second.is_open())
            {
                lv_LogFile.second.flush();
            }
        }

        pm_LogSizeCounter = 0; //reset since all logs have been flushed

        return true;
    }

    //////////////////////////////////////////////////////////// Logging Functions  ////////////////////////////////////////////////////////////

    void
        Logger::Trace
        (
            const string& fp_Message,
            const string& fp_Sender
        )
    {
        if (ValidateLogMsg(PEACH_TRACE_LOG)) //IMPORTANT: don't need to check if the log file was created since activelogmask tracks that as well >w< and the activemask can't be modified directly since its private
        {
            const string f_TimeStamp = GetCurrentTimestamp();
            const string f_LogEntry = 
                fmt::format
                (
                    "[{}][trace][{}]: {}",
                    f_TimeStamp,
                    fp_Sender,
                    fp_Message
                );

            pm_SnapshotBuffer->Emplace(f_TimeStamp, fp_Message, fp_Sender, PEACH_TRACE_LOG);

            if (pm_LogToFile)
            {
                ofstream& f_LogFile = pm_LogFiles.at("trace.log"); //safe to call at() here since its synced at all times w pm_ActiveMask

                if (f_LogFile.is_open())
                {
                    f_LogFile << f_LogEntry << "\n";

                    if (pm_LogSizeCounter++ >= FLUSH_EVERY_N_LOGS)
                    {
                        ForceFlushAllLogs(); //AssertThreadAccess is already called so this is safe UwU >O< !!!!!
                    }
                    else if (pm_FlushMask & FLUSH_TRACE_BIT)
                    {
                        f_LogFile.flush();
                    }
                }
            }

            PRINT(f_LogEntry, Colours::BrightWhite);
        }
    }

    void
        Logger::Debug
        (
            const string& fp_Message,
            const string& fp_Sender
        )
    {
        if (ValidateLogMsg(PEACH_DEBUG_LOG))
        {
            const string f_TimeStamp = GetCurrentTimestamp();
            const string f_LogEntry =
                fmt::format
                (
                    "[{}][debug][{}]: {}",
                    f_TimeStamp,
                    fp_Sender,
                    fp_Message
                );

            pm_SnapshotBuffer->Emplace(f_TimeStamp, fp_Message, fp_Sender, PEACH_DEBUG_LOG);

            if (pm_LogToFile)
            {
                ofstream& f_LogFile = pm_LogFiles.at("debug.log"); // Log to specific log file >W<

                if (f_LogFile.is_open())
                {
                    f_LogFile << f_LogEntry << "\n";

                    if (pm_LogSizeCounter++ >= FLUSH_EVERY_N_LOGS)
                    {
                        ForceFlushAllLogs(); //AssertThreadAccess is already called so this is safe UwU >O< !!!!!
                    }
                    else if (pm_FlushMask & FLUSH_DEBUG_BIT)
                    {
                        f_LogFile.flush();
                    }
                }
            }

            PRINT(f_LogEntry, Colours::BrightBlue);
        }
    }

    void
        Logger::Info
        (
            const string& fp_Message,
            const string& fp_Sender
        )
    {
        if (ValidateLogMsg(PEACH_INFO_LOG))
        {
            const string f_TimeStamp = GetCurrentTimestamp();
            const string f_LogEntry = 
                fmt::format
                (
                    "[{}][info][{}]: {}",
                    f_TimeStamp,
                    fp_Sender,
                    fp_Message
                );

            pm_SnapshotBuffer->Emplace(f_TimeStamp, fp_Message, fp_Sender, PEACH_INFO_LOG);

            if (pm_LogToFile)
            {
                ofstream& f_LogFile = pm_LogFiles.at("info.log"); // Log to specific file and all-logs file

                if (f_LogFile.is_open())
                {
                    f_LogFile << f_LogEntry << "\n";

                    if (pm_LogSizeCounter++ >= FLUSH_EVERY_N_LOGS)
                    {
                        ForceFlushAllLogs(); //AssertThreadAccess is already called so this is safe UwU >O< !!!!!
                    }
                    else if (pm_FlushMask & FLUSH_INFO_BIT)
                    {
                        f_LogFile.flush();
                    }
                }
            }

            PRINT(f_LogEntry, Colours::BrightGreen);
        }
    }

    void
        Logger::Warning
        (
            const string& fp_Message,
            const string& fp_Sender,
            const source_location fp_SourceLocation
        )
    {
        if (ValidateLogMsg(PEACH_WARNING_LOG))
        {
            const string f_TimeStamp = GetCurrentTimestamp();

            const string f_LogEntry = fmt::format
            (
                "[{}][warning][{}] ({} ln {}, {}): {}",
                f_TimeStamp,
                fp_Sender,
                PeachExtractFilename(fp_SourceLocation.file_name()),
                fp_SourceLocation.line(),
                PeachExtractSignature(fp_SourceLocation.function_name()),
                fp_Message
            );

            pm_SnapshotBuffer->Emplace(f_TimeStamp, fp_Message, fp_Sender, PEACH_WARNING_LOG);

            if (pm_LogToFile)
            {
                ofstream& f_LogFile = pm_LogFiles.at("warning.log"); // Log to specific file and all-logs file

                if (f_LogFile.is_open())
                {
                    f_LogFile << f_LogEntry << "\n";

                    if (pm_LogSizeCounter++ >= FLUSH_EVERY_N_LOGS)
                    {
                        ForceFlushAllLogs(); //AssertThreadAccess is already called so this is safe UwU >O< !!!!!
                    }
                    else if (pm_FlushMask & FLUSH_WARNING_BIT)
                    {
                        f_LogFile.flush();
                    }
                }
            }

            PRINT(f_LogEntry, Colours::BrightYellow);
        }
    }

    void
        Logger::Error
        (
            const string& fp_Message,
            const string& fp_Sender,
            const source_location fp_SourceLocation
        )
    {
        if (ValidateLogMsg(PEACH_ERROR_LOG))
        {
            const string f_TimeStamp = GetCurrentTimestamp();

            const string f_LogEntry = fmt::format
            (
                "[{}][error][{}] ({} ln {}, {}): {}",
                f_TimeStamp,
                fp_Sender,
                PeachExtractFilename(fp_SourceLocation.file_name()),
                fp_SourceLocation.line(),
                PeachExtractSignature(fp_SourceLocation.function_name()),
                fp_Message
            );

            pm_SnapshotBuffer->Emplace(f_TimeStamp, fp_Message, fp_Sender, PEACH_ERROR_LOG);

            if (pm_LogToFile)
            {
                ofstream& f_LogFile = pm_LogFiles.at("error.log"); // Log to specific file and all-logs file

                if (f_LogFile.is_open())
                {
                    f_LogFile << f_LogEntry << "\n";

                    if (pm_LogSizeCounter++ >= FLUSH_EVERY_N_LOGS)
                    {
                        ForceFlushAllLogs(); //AssertThreadAccess is already called so this is safe UwU >O< !!!!!
                    }
                    else if (pm_FlushMask & FLUSH_ERROR_BIT)
                    {
                        f_LogFile.flush();
                    }
                }
            }

            PRINT_ERROR(f_LogEntry);
        }
    }

    void
        Logger::Fatal
        (
            const string& fp_Message,
            const string& fp_Sender,
            const source_location fp_SourceLocation
        )
    {
        if (ValidateLogMsg(PEACH_FATAL_LOG))
        {
            const string f_TimeStamp = GetCurrentTimestamp();

            const string f_LogEntry = fmt::format
            (
                "[{}][fatal][{}] ({} ln {}, {}): {}",
                f_TimeStamp,
                fp_Sender,
                PeachExtractFilename(fp_SourceLocation.file_name()),
                fp_SourceLocation.line(),
                PeachExtractSignature(fp_SourceLocation.function_name()),
                fp_Message
            );

            pm_SnapshotBuffer->Emplace(f_TimeStamp, fp_Message, fp_Sender, PEACH_FATAL_LOG);

            if (pm_LogToFile)
            {
                ofstream& f_LogFile = pm_LogFiles.at("fatal.log"); // Log to specific file and all-logs file

                if (f_LogFile.is_open())
                {
                    f_LogFile << f_LogEntry << "\n";

                    if (pm_LogSizeCounter++ >= FLUSH_EVERY_N_LOGS)
                    {
                        ForceFlushAllLogs(); //AssertThreadAccess is already called so this is safe UwU >O< !!!!!
                    }
                    else if (pm_FlushMask & FLUSH_FATAL_BIT)
                    {
                        f_LogFile.flush();
                    }
                }
            }

            PRINT(f_LogEntry, Colours::Magenta);
        }
    }

    bool
        Logger::Initialize
        (
            const string& fp_DesiredLoggerName,
            const string& fp_DesiredOutputDirectory,
            const PEACH_LOGGER_FLAGS fp_Flags
        )
    {
        ////////////////////////////////////////////// Store Initializer Thread ID //////////////////////////////////////////////

        pm_ThreadOwnerID = this_thread::get_id();

        ////////////////////////////////////////////// Set Logger Name + Directory //////////////////////////////////////////////

        pm_LoggerName = fp_DesiredLoggerName;
        pm_CurrentWorkingDirectory = fp_DesiredOutputDirectory + "/" + pm_LoggerName;

        if (fp_Flags & PEACH_LOG_TO_ONLY_SNAPSHOT_BUFFER)
        {
            pm_LogToFile = false;
        }

        ////////////////////////////////////////////// Set Flush Mask //////////////////////////////////////////////

        pm_FlushMask = ExtractFlushMask(fp_Flags);

        ////////////////////////////////////////////// Initialize Snapshot Ring Buffer //////////////////////////////////////////////

        pm_SnapshotBuffer = make_unique<LogBuffer>();

        ////////////////////////////////////////////// Ensure log directory exists //////////////////////////////////////////////

        if (not filesystem::exists(pm_CurrentWorkingDirectory))
        {
            if (fp_Flags & PEACH_DONT_CREATE_DIRECTORY)
            {
                PRINT_ERROR("[CRITICAL_LOGGING_ERROR]: Failed to find valid log output directory");
                return false;
            }

            try
            {
                filesystem::create_directories(pm_CurrentWorkingDirectory); //XXX: this can throw so we wrap it in a try catch
            }
            catch (const exception& f_Exception)
            {
                PRINT_ERROR(fmt::format("Failed to create desired log output directory with exception: '{}'", f_Exception.what()));
                return false;
            }
        }

        ////////////////////////////////////////////// Create Log Files Based on Current Active Mask //////////////////////////////////////////////

        if (not UpdateActiveMask(fp_Flags))
        {
            PRINT_ERROR("[CRITICAL_LOGGING_ERROR]: Failed to create required log files for logger named: " + pm_LoggerName);
            return false;
        }

        ////////////////////////////////////////////// Success! //////////////////////////////////////////////

        return true;
    }

    //////////////////////////////////////////////////////////// Utility Functions  ////////////////////////////////////////////////////////////

    bool
        Logger::CreateLogFile
        (
            const string& fp_FilePath,
            const string& fp_FileName
        )
    {
        ////////////////////////////////////////////// Cache Full Path String //////////////////////////////////////////////

        const string f_FullPath = fp_FilePath + "/" + fp_FileName;

        ////////////////////////////////////////////// If file exists and is too big, truncate it //////////////////////////////////////////////
        error_code f_ErrorCode;

        if (filesystem::exists(f_FullPath, f_ErrorCode) and not f_ErrorCode)
        {
            auto f_LogFileSize = filesystem::file_size(f_FullPath, f_ErrorCode);

            if (not f_ErrorCode and f_LogFileSize >= MAX_LOG_FILE_SIZE_BYTES)
            {
                ////////////////////////////////////////////// truncate by reopening with ios::trunc //////////////////////////////////////////////

                ofstream f_LogFile(f_FullPath, ios::out | ios::trunc);

                if (not f_LogFile.is_open())
                {
                    PRINT_ERROR(fmt::format("Failed to truncate oversized log file: '{}' with logger named: {}", fp_FileName, pm_LoggerName));
                    return false;
                }

                pm_LogFiles[fp_FileName] = move(f_LogFile);

                ////////////////////////////////////////////// Success! //////////////////////////////////////////////

                return true;
            }
        }

        ////////////////////////////////////////////// If file doesn't exist or isn't too big it's business as usual UwU //////////////////////////////////////////////

        ofstream f_LogFile(f_FullPath, ios::out | ios::app);

        if (not f_LogFile.is_open())
        {
            PRINT_ERROR(fmt::format("Failed to open log file: '{}' with logger named: {}", fp_FileName, pm_LoggerName));
            return false;
        }

        pm_LogFiles[fp_FileName] = move(f_LogFile);

        ////////////////////////////////////////////// Success! //////////////////////////////////////////////

        return true;
    }

    void
        Logger::CloseOpenLogFiles()
    {
        for (auto& _f : pm_LogFiles)
        {
            if (_f.second.is_open())
            {
                _f.second.close();
            }
        }
    }

    [[nodiscard]] bool ///XXX: used for testing, this method should never call exit() for a production release, since all logging is hidden away from the game engine dev
        Logger::AssertThreadAccess(const string& fp_FunctionName) //we don't require a lock since this method guarantees only one thread is operating on any data within the Logger instance
        const
    {
        if (this_thread::get_id() == pm_ThreadOwnerID)
        {
            return true;
        }

        stringstream f_UckCPlusPlus; //XXX: cpp is a dumb fucking language sometimes holy please make good features and not dumbass nonsense holy shit
        f_UckCPlusPlus << this_thread::get_id();
        string f_CallerThreadID = f_UckCPlusPlus.str();

        PRINT_ERROR(fmt::format("Logger name: '{}' called method '{}' from the wrong thread, [Caller Thread ID]: {}", pm_LoggerName, fp_FunctionName, f_CallerThreadID));

        return false;
    }

    void
        Logger::ForceFlushAllLogs() //called by functions that already do an AssertThreadAccess call in them uwu this is to avoid double calling OwO!
    {
        for (auto& lv_LogFile : pm_LogFiles)
        {
            if (lv_LogFile.second.is_open())
            {
                lv_LogFile.second.flush();
            }
        }

        pm_LogSizeCounter = 0; //reset since all logs have been flushed
    }
}