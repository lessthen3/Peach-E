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
#include "Managers/LogManager.h"

#include <chrono>
#include <iomanip>
#include <sstream>

#include <filesystem>

constexpr uint8_t TRACE_LOG_INDEX = 0;
constexpr uint8_t DEBUG_LOG_INDEX = 1;
constexpr uint8_t INFO_LOG_INDEX = 2;
constexpr uint8_t WARNING_LOG_INDEX = 3;
constexpr uint8_t ERROR_LOG_INDEX = 4;
constexpr uint8_t FATAL_LOG_INDEX = 5;

//just static functions that the header includes for relevance ig idfk
namespace PeachCore{

#if defined(PEACH_PLATFORM_WINDOWS) && defined(PEACH_USING_OS_TERMINAL)
#   define NOMINMAX
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>

    bool
        EnableWindowsConsoleColours()
    {
        DWORD f_ConsoleMode;
        HANDLE f_OutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);

        if (GetConsoleMode(f_OutputHandle, &f_ConsoleMode))
        {
            SetConsoleMode(f_OutputHandle, f_ConsoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
            return true;
        }
        else
        {
            PEACH_PRINT_ERROR("Was not able to set console mode to allow windows to display ANSI escape codes");
            return false;
        }
    }

#endif /*defined(PEACH_PLATFORM_WINDOWS) && defined(PEACH_USING_OS_TERMINAL)*/

}

namespace PeachCore{ //static internaly linked functions owo

    [[nodiscard]] PEACH_FORCEINLINE static string
        GetCurrentTimestamp()
    {
        const auto now = chrono::system_clock::now();
        auto time_t_now = chrono::system_clock::to_time_t(now);

        tm local_time{}; //what a fucked type name wtf C, the unbridled c programmer urge to name things in the stupidest fucking way lmfao

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

    [[nodiscard]] PEACH_FORCEINLINE static constexpr uint8_t
        ExtractLevelMask(uint32_t fp_Flags) noexcept
    {
        return static_cast<uint8_t>(fp_Flags & 0xFF);
    }

    [[nodiscard]] PEACH_FORCEINLINE static constexpr uint8_t
        ExtractFlushMask(uint32_t fp_Flags) noexcept
    {
        return static_cast<uint8_t>((fp_Flags >> 8) & 0xFF);
    }

    [[nodiscard]] PEACH_FORCEINLINE static constexpr const char*
        PeachExtractFilename(const char* fp_Path)
    {
        const char* f_LastSlash = fp_Path;

        for (const char* lv_Cur = fp_Path; *lv_Cur != '\0'; ++lv_Cur)
        {
            if (*lv_Cur == '/' || *lv_Cur == '\\')
            {
                f_LastSlash = lv_Cur + 1;
            }
        }

        return f_LastSlash;
    }

    [[nodiscard]] PEACH_FORCEINLINE static constexpr std::string_view
        PeachExtractSignature(const char* fp_FunctionSignature)
    {
        std::string_view f_StringView(fp_FunctionSignature);

        // 1. Find the start of the arguments '('
        size_t f_EndIndex = f_StringView.find('(');

        if (f_EndIndex == std::string_view::npos)
        {
            return f_StringView;
        }

        // 2. Look backwards from '(' to find the first space (skipping return type)
        // We want the part between the last space and the '('
        size_t f_StartIndex = 0;

        for (size_t lv_Index = f_EndIndex; lv_Index > 0; --lv_Index)
        {
            if (f_StringView[lv_Index - 1] == ' ')
            {
                f_StartIndex = lv_Index;
                break;
            }
        }

        // 3. Slice it: "void __cdecl Namespace::Class::Func(int)" -> "Namespace::Class::Func"
        return f_StringView.substr(f_StartIndex, f_EndIndex - f_StartIndex);
    }
}

namespace PeachCore{

    static constexpr uint8_t PEACH_LOGGER_FLUSH_TRACE_BIT = 1u << 0;
    static constexpr uint8_t PEACH_LOGGER_FLUSH_DEBUG_BIT = 1u << 1;
    static constexpr uint8_t PEACH_LOGGER_FLUSH_INFO_BIT = 1u << 2;
    static constexpr uint8_t PEACH_LOGGER_FLUSH_WARNING_BIT = 1u << 3;
    static constexpr uint8_t PEACH_LOGGER_FLUSH_ERROR_BIT = 1u << 4;
    static constexpr uint8_t PEACH_LOGGER_FLUSH_FATAL_BIT = 1u << 5;

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
            PEACH_PRINT_ERROR_FMT("Tried to call UpdateThreadOwner from a thread that didn't own logger named: {}", pm_LoggerName);
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

        ////////////////////////////////////////////// Reset Mask //////////////////////////////////////////////

        pm_ActiveLogMask = 0;

        ////////////////////////////////////////////// Create Log files based off of Current active mask uwu //////////////////////////////////////////////

        for (size_t lv_CurrentLogFileIndex = 0; lv_CurrentLogFileIndex < 6; lv_CurrentLogFileIndex++)
        {
            if (fp_NewLogMask & lv_LogEnum)
            {
                if (not CreateLogFile(pm_CurrentWorkingDirectory, lv_LogStringName))
                {
                    PEACH_PRINT_ERROR_FMT("Failed to create log file named: {}", lv_LogStringName);
                    return false;
                }

                pm_ActiveLogMask |= static_cast<uint8_t>(lv_LogEnum);
            }
        }

        ////////////////////////////////////////////// Success! //////////////////////////////////////////////

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

            pm_SnapshotBuffer.Emplace(f_TimeStamp, fp_Message, fp_Sender, PEACH_TRACE_LOG);

            if (pm_LogToFile)
            {
                if (f_LogFile.is_open())
                {
                    f_LogFile << f_LogEntry << "\n";

                    if (pm_LogSizeCounter++ >= PEACH_LOGGER_FLUSH_EVERY_N_LOGS)
                    {
                        ForceFlushAllLogs(); //AssertThreadAccess is already called so this is safe UwU >O< !!!!!
                    }
                    else if (pm_FlushMask & PEACH_LOGGER_FLUSH_TRACE_BIT)
                    {
                        f_LogFile.flush();
                    }
                }
            }

            PEACH_PRINT(f_LogEntry, PEACH_COL_BRIGHT_WHITE);
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

            pm_SnapshotBuffer.Emplace(f_TimeStamp, fp_Message, fp_Sender, PEACH_DEBUG_LOG);

            if (pm_LogToFile)
            {
                if (f_LogFile.is_open())
                {
                    f_LogFile << f_LogEntry << "\n";

                    if (pm_LogSizeCounter++ >= PEACH_LOGGER_FLUSH_EVERY_N_LOGS)
                    {
                        ForceFlushAllLogs(); //AssertThreadAccess is already called so this is safe UwU >O< !!!!!
                    }
                    else if (pm_FlushMask & PEACH_LOGGER_FLUSH_DEBUG_BIT)
                    {
                        f_LogFile.flush();
                    }
                }
            }

            PEACH_PRINT(f_LogEntry, PEACH_COL_BRIGHT_BLUE);
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

            pm_SnapshotBuffer.Emplace(f_TimeStamp, fp_Message, fp_Sender, PEACH_INFO_LOG);

            if (pm_LogToFile)
            {

                if (f_LogFile.is_open())
                {
                    f_LogFile << f_LogEntry << "\n";

                    if (pm_LogSizeCounter++ >= PEACH_LOGGER_FLUSH_EVERY_N_LOGS)
                    {
                        ForceFlushAllLogs(); //AssertThreadAccess is already called so this is safe UwU >O< !!!!!
                    }
                    else if (pm_FlushMask & PEACH_LOGGER_FLUSH_INFO_BIT)
                    {
                        f_LogFile.flush();
                    }
                }
            }

            PEACH_PRINT(f_LogEntry, PEACH_COL_BRIGHT_GREEN);
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

            pm_SnapshotBuffer.Emplace(f_TimeStamp, fp_Message, fp_Sender, PEACH_WARNING_LOG);

            if (pm_LogToFile)
            {
                if (f_LogFile.is_open())
                {
                    f_LogFile << f_LogEntry << "\n";

                    if (pm_LogSizeCounter++ >= PEACH_LOGGER_FLUSH_EVERY_N_LOGS)
                    {
                        ForceFlushAllLogs(); //AssertThreadAccess is already called so this is safe UwU >O< !!!!!
                    }
                    else if (pm_FlushMask & PEACH_LOGGER_FLUSH_WARNING_BIT)
                    {
                        f_LogFile.flush();
                    }
                }
            }

            PEACH_PRINT(f_LogEntry, PEACH_COL_BRIGHT_YELLOW);
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

            pm_SnapshotBuffer.Emplace(f_TimeStamp, fp_Message, fp_Sender, PEACH_ERROR_LOG);

            if (pm_LogToFile)
            {
                if (f_LogFile.is_open())
                {
                    f_LogFile << f_LogEntry << "\n";

                    if (pm_LogSizeCounter++ >= PEACH_LOGGER_FLUSH_EVERY_N_LOGS)
                    {
                        ForceFlushAllLogs(); //AssertThreadAccess is already called so this is safe UwU >O< !!!!!
                    }
                    else if (pm_FlushMask & PEACH_LOGGER_FLUSH_ERROR_BIT)
                    {
                        f_LogFile.flush();
                    }
                }
            }

            PEACH_PRINT_ERROR(f_LogEntry);
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

            pm_SnapshotBuffer.Emplace(f_TimeStamp, fp_Message, fp_Sender, PEACH_FATAL_LOG);

            if (pm_LogToFile)
            {
                if (f_LogFile.is_open())
                {
                    f_LogFile << f_LogEntry << "\n";

                    if (pm_LogSizeCounter++ >= PEACH_LOGGER_FLUSH_EVERY_N_LOGS)
                    {
                        ForceFlushAllLogs(); //AssertThreadAccess is already called so this is safe UwU >O< !!!!!
                    }
                    else if (pm_FlushMask & PEACH_LOGGER_FLUSH_FATAL_BIT)
                    {
                        f_LogFile.flush();
                    }
                }
            }

            PEACH_PRINT(f_LogEntry, PEACH_COL_MAGENTA);
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

        ////////////////////////////////////////////// Ensure log directory exists //////////////////////////////////////////////

        if (not filesystem::exists(pm_CurrentWorkingDirectory))
        {
            if (fp_Flags & PEACH_DONT_CREATE_DIRECTORY)
            {
                PEACH_PRINT_ERROR("[CRITICAL_LOGGING_ERROR]: Failed to find valid log output directory");
                return false;
            }

            try
            {
                filesystem::create_directories(pm_CurrentWorkingDirectory); //XXX: this can throw so we wrap it in a try catch
            }
            catch (const exception& f_Exception)
            {
                PEACH_PRINT_ERROR_FMT("Failed to create desired log output directory with exception: '{}'", f_Exception.what());
                return false;
            }
        }

        ////////////////////////////////////////////// Create Log Files Based on Current Active Mask //////////////////////////////////////////////

        if (not UpdateActiveMask(fp_Flags))
        {
            PEACH_PRINT_ERROR_FMT("[CRITICAL_LOGGING_ERROR]: Failed to create required log files for logger named: {}", pm_LoggerName);
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

            if (not f_ErrorCode and f_LogFileSize >= PEACH_LOGGER_MAX_LOG_FILE_SIZE_BYTES)
            {
                ////////////////////////////////////////////// truncate by reopening with ios::trunc //////////////////////////////////////////////

                ofstream f_LogFile(f_FullPath, ios::out | ios::trunc);

                if (not f_LogFile.is_open())
                {
                    PEACH_PRINT_ERROR_FMT("Failed to truncate oversized log file: '{}' with logger named: {}", fp_FileName, pm_LoggerName);
                    return false;
                }

                pm_LogFiles[fp_FileName] = std::move(f_LogFile);

                ////////////////////////////////////////////// Success! //////////////////////////////////////////////

                return true;
            }
        }

        ////////////////////////////////////////////// If file doesn't exist or isn't too big it's business as usual UwU //////////////////////////////////////////////

        ofstream f_LogFile(f_FullPath, ios::out | ios::app);

        if (not f_LogFile.is_open())
        {
            PEACH_PRINT_ERROR_FMT("Failed to open log file: '{}' with logger named: {}", fp_FileName, pm_LoggerName);
            return false;
        }

        pm_LogFiles[fp_FileName] = std::move(f_LogFile);

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

    PEACH_FORCEINLINE bool ///XXX: Force inline here since it's only used within this TU
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

        PEACH_PRINT_ERROR_FMT("Logger name: '{}' called method '{}' from the wrong thread, [Caller Thread ID]: {}", pm_LoggerName, fp_FunctionName, f_CallerThreadID);

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