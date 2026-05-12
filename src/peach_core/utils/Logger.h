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

#define PEACH_LOGGER_DEFAULT_FLAGS PEACH_ALL_LOGS | PEACH_FLUSH_ERROR

///IMPORTANT: ur cute >w<

#ifdef PEACH_DEBUG // can always just do a ptr deref since the factory functions guarantee Logger can only be heap allocated owo
#   define PEACH_LOG_DEBUG(logger, msg, sender) logger->Debug(msg, sender)  
#   define PEACH_LOG_TRACE(logger, msg, sender) logger->Trace(msg, sender)
#else
#   define PEACH_LOG_DEBUG(logger, msg, sender) ((void)0)
#   define PEACH_LOG_TRACE(logger, msg, sender) ((void)0)
#endif /*PEACH_DEBUG*/

/// STL
#include <string>
#include <thread>
#include <source_location>
#include <mutex>

///PeachCore
#include "RingBuffer.h"
#include "peach_api/LoggerFlags.h"

///fmt
#include "PeachPrint.h"
#include "PeachForceInline.h"

constexpr int FATAL_SEGMENTATION_FAULT = -6969;

/// moody camel queue size uwu
constexpr unsigned int MOODY_CAMEL_QUEUE_SIZE = 128u;

static constexpr uint32_t PEACH_LOGGER_MAX_NUMBER_OF_LOGS = 1024u;
static constexpr uint32_t PEACH_LOGGER_FLUSH_EVERY_N_LOGS = 256u;
static constexpr uint32_t PEACH_LOGGER_MAX_LOG_FILE_SIZE_BYTES = 10u * 1024u * 1024u; // 10 MB

#if defined(PEACH_PLATFORM_WINDOWS) && defined(PEACH_USING_OS_TERMINAL)

[[nodiscard]] bool
    PEACH_EnableWindowsConsoleColours();

#endif

namespace PeachCore {

    using namespace std; //this should be here so i dont affect anybody who links against peach

    //////////////////////////////////////////////
    // LogMessage Struct
    //////////////////////////////////////////////

    struct LogMessage
    {
        string Timestamp;  // "2025-01-01 13:37:00.123"
        string Message;
        string Sender;
        uint8_t Level;

        [[nodiscard]] PEACH_FORCEINLINE string
            Formatted(const string& fp_LevelName)
            const
        {
            return fmt::format("[{}][{}][{}]: {}", Timestamp, fp_LevelName, Sender, Message);
        }
    };

    //////////////////////////////////////////////
    // Logger Class
    //////////////////////////////////////////////

    struct LogManager; //just trust he's gonna come ma boy w a keg

    class Logger
    {
        //////////////////////////////////////////////
        // Public Destructor
        //////////////////////////////////////////////
    public:
        ~Logger(); ///XXX: Just copy and pasted the flushalllogs method because they have the assert at the beginning and wont work with premature exit

        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;
        Logger& operator=(Logger&&) = delete;
        Logger(Logger&&) = delete;

        friend LogManager;

    public:
        using LogBuffer = RingBuffer<LogMessage, PEACH_LOGGER_MAX_NUMBER_OF_LOGS>; //XXX: can stack alloc since logger can only be created on da heap owo

        //////////////////////////////////////////////
        // Protected Constructor
        //////////////////////////////////////////////
    protected:
        Logger() = default;

        //////////////////////////////////////////////
        // Protected Class Members
        //////////////////////////////////////////////
    protected:
        array<FILE*, 6> pm_LogFileHandles{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}; //only 6 log levels, one slot per log file owo
        LogBuffer pm_SnapshotBuffer; //no heap alloc since Logger can only be instantiated on the heap since private ctor and factory functions owo

        string pm_LoggerName = "No_Logger_Name";
        string pm_CurrentWorkingDirectory = "nothing";

        thread::id pm_ThreadOwnerID;

        uint8_t pm_ActiveLogMask = 0;
        uint8_t pm_FlushMask = 0;

        bool pm_LogToFile = true;

        uint32_t pm_LogSizeCounter = 0;

        std::mutex pm_LogFileMutex;

        //////////////////////////////////////////////
        // Public Methods
        //////////////////////////////////////////////
    public:

        [[nodiscard]] bool
            UpdateThreadOwner //the owning thread must update and pass off the logger to be considered valid otherwise it wont uwu
            (
                const thread::id& fp_NewThreadID
            );

        [[nodiscard]] bool
            UpdateActiveMask(const uint32_t fp_NewLogMask);

        const LogBuffer&
            GetSnapshotBuffer()
            const noexcept
        {
            return pm_SnapshotBuffer;
        }

        //////////////////////////////////////////////////////////// Logging Functions  ////////////////////////////////////////////////////////////

        #ifdef PEACH_DEBUG
            void
                Trace
                (
                    const string& fp_Message,
                    const string& fp_Sender
                );

            void
                Debug
                (
                    const string& fp_Message,
                    const string& fp_Sender
                );
        #endif /*PEACH_DEBUG*/

        void
            Info
            (
                const string& fp_Message,
                const string& fp_Sender
            );

        void
            Warning
            (
                const string& fp_Message,
                const string& fp_Sender,
                const source_location fp_SourceLocation = source_location::current()
            );

        void
            Error
            (
                const string& fp_Message,
                const string& fp_Sender,
                const source_location fp_SourceLocation = source_location::current()
            );

        void
            Fatal
            (
                const string& fp_Message,
                const string& fp_Sender,
                const source_location fp_SourceLocation = source_location::current()
            );

        //////////////////////////////////////////////
        // Protected Methods
        //////////////////////////////////////////////
    protected:
        [[nodiscard]] bool
            Initialize
            (
                const string& fp_DesiredLoggerName,
                const string& fp_DesiredOutputDirectory,
                const PEACH_LOGGER_FLAGS fp_Flags
            );

        [[nodiscard]] PEACH_FORCEINLINE bool
            ValidateLogMsg(const uint8_t fp_LogLevel)
        {
            return (AssertThreadAccess("ValidateLogMsg") and pm_ActiveLogMask & fp_LogLevel); //return early without logging if loglevel isnt active or hasnt been initialized or if accessed from the wrong thread
        }

        [[nodiscard]] bool
            CreateLogFile
            (
                const std::string& fp_FilePath,
                const std::string& fp_FileName,
                uint8_t fp_LevelIndex
            );

        void
            CloseOpenLogFiles()
            noexcept;

        [[nodiscard]] bool ///XXX: used for testing, this method should never call exit() for a production release, since all logging is hidden away from the game engine dev
            AssertThreadAccess(const string& fp_FunctionName) //we don't require a lock since this method guarantees only one thread is operating on any data within the Logger instance
            const;

        void
            FlushAllLocked()
            noexcept; //locks the mutex so that on panic we dont get any weird UB
            
        void
            WriteLogEntry
            (
                const uint8_t fp_LevelIndex, 
                const uint8_t fp_FlushBit,
                const std::string& fp_Entry
            );
    };
}