/*******************************************************************
 *                        Peach-E v0.0.1
 *              Created by Ranyodh Mandur - 🍑 2024
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:         
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#pragma once

#define PEACH_ARRAY_SIZE(x) sizeof(x) / sizeof(x[0]) 

#define PEACH_MAX_PTR_DIFF std::numeric_limits<ptrdiff_t>::max()

#ifdef PEACH_DEBUG //TEMPORARY JUST HERE FOR NOW TESTING THE IDEA, SINCE DEBUG DOESNT ALWAYS MEAN USING TERMINAL
    #define PEACH_USING_OS_TERMINAL
#endif

#if defined(_WIN32) || defined(_WIN64)
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN

    #include <windows.h>
#endif

#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>

#include <chrono>
#include <iomanip>
#include <sstream>

#include <unordered_map>
#include <format>
#include <memory>

#include <thread>

#include <optional>

#include "../Utils/RingBuffer.h"

//#ifndef $ //Used for bookkeeping and tracking what variables are mutable inside a function
//#define $
//#endif

constexpr const int FAILED_TO_CREATE_MAIN_WINDOW = -1000;
constexpr const int FAILED_TO_INITIALIZE_OPENGL = -1001;
constexpr const int FAILED_TO_INITIALIZE_VULKAN = -1002;

constexpr const int FATAL_SEGMENTATION_FAULT = -6969;

/// moody camel queue size uwu
constexpr const unsigned int MOODY_CAMEL_QUEUE_SIZE = 128;

namespace PeachCore {

    using namespace std;

    #if (defined(_WIN32) || defined(_WIN64)) && defined(PEACH_USING_OS_TERMINAL)

        static bool
            EnableColors()
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
                cerr << ("Was not able to set console mode to allow windows to display ANSI escape codes") << "\n";
                return false;
            }
        }

    #endif

    enum class Colours : int
    {
        Black,
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,
        Cyan,
        White,

        BrightBlack,
        BrightRed,
        BrightGreen,
        BrightYellow,
        BrightBlue,
        BrightMagenta,
        BrightCyan,
        BrightWhite
    };

    [[nodiscard]] constexpr string 
        CreateColouredText
        (
            const string& fp_SampleText,
            const Colours fp_DesiredColour
        )
    {
        switch (fp_DesiredColour)
        {
            //////////////////// Regular Colours ////////////////////

        case Colours::Black: return "\x1B[30m" + fp_SampleText + "\033[0m";

        case Colours::Red: return "\x1B[31m" + fp_SampleText + "\033[0m";

        case Colours::Green: return "\x1B[32m" + fp_SampleText + "\033[0m";

        case Colours::Yellow: return "\x1B[33m" + fp_SampleText + "\033[0m";

        case Colours::Blue: return "\x1B[34m" + fp_SampleText + "\033[0m";

        case Colours::Magenta: return "\x1B[35m" + fp_SampleText + "\033[0m";

        case Colours::Cyan: return "\x1B[36m" + fp_SampleText + "\033[0m";

        case Colours::White: return "\x1B[37m" + fp_SampleText + "\033[0m";


            //////////////////// Bright Colours ////////////////////

        case Colours::BrightBlack: return "\x1B[90m" + fp_SampleText + "\033[0m";

        case Colours::BrightRed: return "\x1B[91m" + fp_SampleText + "\033[0m";

        case Colours::BrightGreen: return "\x1B[92m" + fp_SampleText + "\033[0m";

        case Colours::BrightYellow: return "\x1B[93m" + fp_SampleText + "\033[0m";

        case Colours::BrightBlue: return "\x1B[94m" + fp_SampleText + "\033[0m";

        case Colours::BrightMagenta: return "\x1B[95m" + fp_SampleText + "\033[0m";

        case Colours::BrightCyan: return "\x1B[96m" + fp_SampleText + "\033[0m";

        case Colours::BrightWhite: return "\x1B[97m" + fp_SampleText + "\033[0m";

            //////////////////// Just Return the Input Text Unaltered Otherwise ////////////////////

        default: return fp_SampleText;
        }
    }

    static void
        Print
        (
            const string& fp_Message,
            const Colours fp_DesiredColour = Colours::White
        )
    {
        cout << CreateColouredText(fp_Message, fp_DesiredColour) << "\n";
    }

    static void
        PrintError
        (
            const string& fp_Message,
            const Colours fp_DesiredColour = Colours::Red
        )
    {
        cerr << CreateColouredText(fp_Message, fp_DesiredColour) << "\n";
    }

    //////////////////////////////////////////////
    // LogMessage Struct
    //////////////////////////////////////////////

    struct LogMessage
    {
        string Timestamp;  // "2025-01-01 13:37:00.123"
        string Message;
        string Sender;
        uint8_t Level;

        [[nodiscard]] string
            Formatted(const string& fp_LevelName) 
            const
        {
            return "[" + Timestamp + "][" + fp_LevelName + "][" + Sender + "]: " + Message;
        }
    };

    //////////////////////////////////////////////
    // Logger Class
    //////////////////////////////////////////////

    class Logger
    {
        //////////////////////////////////////////////
        // Public Destructor
        //////////////////////////////////////////////
    public:
        ~Logger() ///XXX: Just copy and pasted the flushalllogs method because they have the assert at the beginning and wont work with premature exit
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

        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;
        Logger& operator=(Logger&&) = delete;

        /*
            needed for stack allocated Create(), allows for nrvo and also is kosher since move constructors play w the strict ownership model that is the foundation of the thread owning system uwu
            so the pattern is using optional return a nrvo Logger, and the thread thats using it calls Create() so this_thread::thread::id works properly ^_^
        */
        Logger(Logger&&) = default; 

        static constexpr uint32_t FLUSH_EVERY_N_LOGS = 256;
        static constexpr uint32_t MAX_NUMBER_OF_LOGS = 1024;
        static constexpr uintmax_t MAX_LOG_FILE_SIZE_BYTES = 10 * 1024 * 1024; // 10 MB

        static constexpr uint8_t FLUSH_TRACE_BIT = 1u << 0;
        static constexpr uint8_t FLUSH_DEBUG_BIT = 1u << 1;
        static constexpr uint8_t FLUSH_INFO_BIT = 1u << 2;
        static constexpr uint8_t FLUSH_WARNING_BIT = 1u << 3;
        static constexpr uint8_t FLUSH_ERROR_BIT = 1u << 4;
        static constexpr uint8_t FLUSH_FATAL_BIT = 1u << 5;


        //////////////////////////////////////////////
        // Protected Constructor
        //////////////////////////////////////////////
    protected:
        Logger() = default;

        ////////////////////////////////////////////////
        // Helper Enum For LogLevel Specification
        ////////////////////////////////////////////////
    public:

        enum Flags : uint32_t
        {
            // low byte is active mask
            TRACE_LOG = 1u << 0,
            DEBUG_LOG = 1u << 1,
            INFO_LOG = 1u << 2,
            WARNING_LOG = 1u << 3,
            ERROR_LOG = 1u << 4,
            FATAL_LOG = 1u << 5,

            ALL_LOGS = TRACE_LOG | DEBUG_LOG | INFO_LOG | WARNING_LOG | ERROR_LOG | FATAL_LOG,

            //middle byte is flush mask
            FLUSH_TRACE =  1u << 8,
            FLUSH_DEBUG = 1u << 9,
            FLUSH_INFO = 1u << 10,
            FLUSH_WARNING = 1u << 11,
            FLUSH_ERROR = 1u << 12,
            FLUSH_FATAL = 1u << 13,

            FLUSH_ALL = FLUSH_TRACE | FLUSH_DEBUG | FLUSH_INFO | FLUSH_WARNING | FLUSH_ERROR | FLUSH_FATAL,

            //high byte is aux flags
            DONT_CREATE_DIRECTORY = 1u << 16,
            LOG_TO_ONLY_SNAPSHOT_BUFFER = 1u << 17
        };

        //////////////////////////////////////////////
        // Protected Class Members
        //////////////////////////////////////////////
    protected:
        unordered_map<string, ofstream> pm_LogFiles;

        unique_ptr<RingBuffer<LogMessage, MAX_NUMBER_OF_LOGS>> pm_SnapshotBuffer = nullptr;

        string pm_LoggerName = "No_Logger_Name";
        string pm_CurrentWorkingDirectory = "nothing";

        thread::id pm_ThreadOwnerID;

        uint8_t pm_ActiveLogMask = 0;
        uint8_t pm_FlushMask = 0;

        bool pm_LogToFile = true;

        uint32_t pm_LogSizeCounter = 0;

        //////////////////////////////////////////////
        // Public Methods
        //////////////////////////////////////////////
    public:

        [[nodiscard]] static optional<Logger>
            Create
            (
                const string& fp_DesiredLoggerName,
                const uint32_t fp_Flags,
                const string& fp_DesiredOutputDirectory = ""
            )
        {
            Logger f_CreatedLogger; 

            if (not f_CreatedLogger.Initialize(fp_DesiredLoggerName, fp_DesiredOutputDirectory, fp_Flags))
            {
                PrintError("Unable to initialize logger named: " + fp_DesiredLoggerName);
                return nullopt;
            }

            return f_CreatedLogger; //NRVO
        }

        [[nodiscard]] static unique_ptr<Logger>
            CreateUnique
            (
                const string& fp_DesiredLoggerName,
                const uint32_t fp_Flags,
                const string& fp_DesiredOutputDirectory = ""
            )
        {
            unique_ptr<Logger> f_CreatedLogger(new Logger()); //this is dumb but std doesn't like my private constructor uwu!

            if (not f_CreatedLogger->Initialize(fp_DesiredLoggerName, fp_DesiredOutputDirectory, fp_Flags))
            {
                PrintError("Unable to initialize logger named: " + fp_DesiredLoggerName);
                return nullptr;
            }

            return move(f_CreatedLogger);
        }

        [[nodiscard]] static shared_ptr<Logger>
            CreateShared
            (
                const string& fp_DesiredLoggerName,
                const uint32_t fp_Flags,
                const string& fp_DesiredOutputDirectory = ""
            )
        {
            shared_ptr<Logger> f_CreatedLogger(new Logger()); //this is dumb but std doesn't like my private constructor uwu!

            if (not f_CreatedLogger->Initialize(fp_DesiredLoggerName, fp_DesiredOutputDirectory, fp_Flags))
            {
                PrintError("Unable to initialize logger named: " + fp_DesiredLoggerName);
                return nullptr;
            }

            return f_CreatedLogger;
        }

        bool
            UpdateThreadOwner //the owning thread must update and pass off the logger to be considered valid otherwise it wont uwu
            (
                const thread::id& fp_NewThreadID
            ) 
        {
            if (not AssertThreadAccess("UpdateThreadOwner"))
            {
                //can't log here since it's only triggered by improper thread usage which will trigger asserthreadacess again
                PrintError(format("Tried to call UpdateThreadOwner from a thread that didn't own logger named: {}", pm_LoggerName)); 
                return false;
            }

            pm_ThreadOwnerID = fp_NewThreadID;

            return true;
        }

        [[nodiscard]] bool 
            UpdateActiveMask(const uint32_t fp_NewLogMask)
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
                {TRACE_LOG, "trace.log"},
                {DEBUG_LOG, "debug.log"},
                {INFO_LOG, "info.log"},
                {WARNING_LOG, "warning.log"},
                {ERROR_LOG, "error.log"},
                {FATAL_LOG, "fatal.log"}
            };

            for (const auto& [lv_LogEnum, lv_LogStringName] : f_LogLevels)
            {
                if (fp_NewLogMask & lv_LogEnum)
                {
                    if (not CreateLogFile(pm_CurrentWorkingDirectory, lv_LogStringName))
                    {
                        PrintError("Failed to create log file named: " + lv_LogStringName);
                        return false;
                    }

                    pm_ActiveLogMask |= static_cast<uint8_t>(lv_LogEnum);
                }
            }

            ////////////////////////////////////////////// Success! //////////////////////////////////////////////

            return true;
        }

        const RingBuffer<LogMessage, MAX_NUMBER_OF_LOGS>&
            GetSnapshotBuffer() 
            const noexcept
        {
            return *pm_SnapshotBuffer;
        }


        //////////////////// Flush All Logs ////////////////////

        [[nodiscard]] bool
            FlushAllLogs()
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

        [[nodiscard]] bool
            ValidateLogMsg(const uint8_t fp_LogLevel)
        {
            return (AssertThreadAccess("ValidateLogMsg") and pm_ActiveLogMask & fp_LogLevel); //return early without logging if loglevel isnt active or hasnt been initialized or if accessed from the wrong thread
        }

        //////////////////////////////////////////////////////////// Logging Functions  ////////////////////////////////////////////////////////////

        void
            Trace
            (
                const string& fp_Message, 
                const string& fp_Sender
            )
        {
            if (ValidateLogMsg(static_cast<uint8_t>(Flags::TRACE_LOG))) //IMPORTANT: don't need to check if the log file was created since activelogmask tracks that as well >w< and the activemask can't be modified directly since its private
            {
                const string f_TimeStamp = GetCurrentTimestamp();
                const string f_LogEntry = "[" + f_TimeStamp + "][trace][" + fp_Sender + "]: " + fp_Message;

                pm_SnapshotBuffer->Emplace(f_TimeStamp, fp_Message, fp_Sender, static_cast<uint8_t>(Flags::TRACE_LOG));

                if (pm_LogToFile)
                {
                    ofstream& f_LogFile = pm_LogFiles.at("trace.log"); //safe to call at() here since its synced at all times w pm_ActiveMask

                    if(f_LogFile.is_open())
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

                #ifdef PEACH_USING_OS_TERMINAL
                    Print(f_LogEntry, Colours::BrightWhite);
                #endif
            }
        }

        void 
            Debug
            (
                const string& fp_Message,
                const string& fp_Sender
            )
        {
            if (ValidateLogMsg(static_cast<uint8_t>(Flags::DEBUG_LOG)))
            {
                const string f_TimeStamp = GetCurrentTimestamp();
                const string f_LogEntry = "[" + f_TimeStamp + "][debug][" + fp_Sender + "]: " + fp_Message;

                pm_SnapshotBuffer->Emplace(f_TimeStamp, fp_Message, fp_Sender, static_cast<uint8_t>(Flags::DEBUG_LOG));

                if (pm_LogToFile)
                {
                    ofstream& f_LogFile = pm_LogFiles.at("debug.log"); // Log to specific log file >W<

                    if(f_LogFile.is_open())
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

                #ifdef PEACH_USING_OS_TERMINAL
                    Print(f_LogEntry, Colours::BrightBlue);
                #endif
            }
        }

        void
            Info
            (
                const string& fp_Message,
                const string& fp_Sender
            )
        {
            if (ValidateLogMsg(static_cast<uint8_t>(Flags::INFO_LOG)))
            {
                const string f_TimeStamp = GetCurrentTimestamp();
                const string f_LogEntry = "[" + f_TimeStamp + "][info][" + fp_Sender + "]: " + fp_Message;

                pm_SnapshotBuffer->Emplace(f_TimeStamp, fp_Message, fp_Sender, static_cast<uint8_t>(Flags::INFO_LOG));

                if (pm_LogToFile)
                {
                    ofstream& f_LogFile = pm_LogFiles.at("info.log"); // Log to specific file and all-logs file

                    if(f_LogFile.is_open())
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

                #ifdef PEACH_USING_OS_TERMINAL
                    Print(f_LogEntry, Colours::BrightGreen);
                #endif
            }
        }

        void
            Warning
            (
                const string& fp_Message,
                const string& fp_Sender
            )
        {
            if (ValidateLogMsg(static_cast<uint8_t>(Flags::WARNING_LOG)))
            {
                const string f_TimeStamp = GetCurrentTimestamp();
                const string f_LogEntry = "[" + f_TimeStamp + "][warning][" + fp_Sender + "]: " + fp_Message;

                pm_SnapshotBuffer->Emplace(f_TimeStamp, fp_Message, fp_Sender, static_cast<uint8_t>(Flags::WARNING_LOG));

                if (pm_LogToFile)
                {
                    ofstream& f_LogFile = pm_LogFiles.at("warning.log"); // Log to specific file and all-logs file

                    if(f_LogFile.is_open())
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

                #ifdef PEACH_USING_OS_TERMINAL
                    Print(f_LogEntry, Colours::BrightYellow);
                #endif
            }
        }

        void
            Error
            (
                const string& fp_Message,
                const string& fp_Sender
            )
        {
            if (ValidateLogMsg(static_cast<uint8_t>(Flags::ERROR_LOG)))
            {
                const string f_TimeStamp = GetCurrentTimestamp();
                const string f_LogEntry = "[" + f_TimeStamp + "][error][" + fp_Sender + "]: " + fp_Message;

                pm_SnapshotBuffer->Emplace(f_TimeStamp, fp_Message, fp_Sender, static_cast<uint8_t>(Flags::ERROR_LOG));

                if (pm_LogToFile)
                {
                    ofstream& f_LogFile = pm_LogFiles.at("error.log"); // Log to specific file and all-logs file

                    if(f_LogFile.is_open())
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

                #ifdef PEACH_USING_OS_TERMINAL
                    PrintError(f_LogEntry, Colours::Red);
                #endif
            }
        }

        void
            Fatal
            (
                const string& fp_Message,
                const string& fp_Sender
            )
        {
            if (ValidateLogMsg(static_cast<uint8_t>(Flags::FATAL_LOG)))
            {
                const string f_TimeStamp = GetCurrentTimestamp();
                const string f_LogEntry = "[" + f_TimeStamp + "][fatal][" + fp_Sender + "]: " + fp_Message;

                pm_SnapshotBuffer->Emplace(f_TimeStamp, fp_Message, fp_Sender, static_cast<uint8_t>(Flags::FATAL_LOG));

                if (pm_LogToFile)
                {
                    ofstream& f_LogFile = pm_LogFiles.at("fatal.log"); // Log to specific file and all-logs file

                    if(f_LogFile.is_open())
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

                #ifdef PEACH_USING_OS_TERMINAL
                    PrintError(f_LogEntry, Colours::Magenta);
                #endif
            }
        }

    //////////////////////////////////////////////
    // Protected Methods
    //////////////////////////////////////////////
    protected:
        [[nodiscard]] bool
            Initialize
            (
                const string& fp_DesiredLoggerName,
                const string& fp_DesiredOutputDirectory,
                const uint32_t fp_Flags
            )
        {
#ifdef PEACH_DEBUG
            //stringstream f_UckCPlusPlus; //XXX: cpp is a dumb fucking language sometimes holy please make good features and not dumbass nonsense holy shit
            //f_UckCPlusPlus << this_thread::get_id();
            //string f_CallerThreadID = f_UckCPlusPlus.str();

            //PrintError(format("Logger name: '{}' from thread number : {}, [Caller Thread ID]: {}", fp_DesiredLoggerName, static_cast<uint8_t>(fp_ThreadName), f_CallerThreadID));
#endif
            ////////////////////////////////////////////// Store Initializer Thread ID //////////////////////////////////////////////

            pm_ThreadOwnerID = this_thread::get_id();

            ////////////////////////////////////////////// Set Logger Name + Directory //////////////////////////////////////////////

            pm_LoggerName = fp_DesiredLoggerName;
            pm_CurrentWorkingDirectory = fp_DesiredOutputDirectory + "/" + pm_LoggerName;

            if (fp_Flags & Flags::LOG_TO_ONLY_SNAPSHOT_BUFFER)
            {
                pm_LogToFile = false;
            }

            ////////////////////////////////////////////// Set Flush Mask //////////////////////////////////////////////

            pm_FlushMask = ExtractFlushMask(fp_Flags);

            ////////////////////////////////////////////// Initialize Snapshot Ring Buffer //////////////////////////////////////////////

            pm_SnapshotBuffer = make_unique<RingBuffer<LogMessage, MAX_NUMBER_OF_LOGS>>();

            ////////////////////////////////////////////// Ensure log directory exists //////////////////////////////////////////////

            if (not filesystem::exists(pm_CurrentWorkingDirectory))
            {
                if(fp_Flags & Flags::DONT_CREATE_DIRECTORY)
                {
                    PrintError("[CRITICAL_LOGGING_ERROR]: Failed to find valid log output directory");
                    return false;
                }

                try
                {
                    filesystem::create_directories(pm_CurrentWorkingDirectory); //XXX: this can throw so we wrap it in a try catch
                }
                catch (const exception& f_Exception)
                {
                    PrintError(format("Failed to create desired log output directory with exception: '{}'", f_Exception.what()));
                    return false;
                }
            }

            ////////////////////////////////////////////// Create Log Files Based on Current Active Mask //////////////////////////////////////////////

            if (not UpdateActiveMask(fp_Flags))
            {
                PrintError("[CRITICAL_LOGGING_ERROR]: Failed to create required log files for logger named: " + pm_LoggerName);
                return false;
            }

            ////////////////////////////////////////////// Success! //////////////////////////////////////////////

            return true;
        }

        //////////////////////////////////////////////////////////// Utility Functions  ////////////////////////////////////////////////////////////

        [[nodiscard]] bool
            CreateLogFile
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
                        PrintError(format("Failed to truncate oversized log file: '{}' with logger named: {}", fp_FileName, pm_LoggerName));
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
                PrintError(format("Failed to open log file: '{}' with logger named: {}", fp_FileName, pm_LoggerName));
                return false;
            }

            pm_LogFiles[fp_FileName] = move(f_LogFile);

            ////////////////////////////////////////////// Success! //////////////////////////////////////////////

            return true; 
        }

        [[nodiscard]] inline string //thank you chat-gpt uwu
            GetCurrentTimestamp()
            const
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

        void
            CloseOpenLogFiles()
        {
            for (auto& _f : pm_LogFiles)
            {
                if (_f.second.is_open())
                {
                    _f.second.close();
                }
            }
        }

        [[nodiscard]] inline bool ///XXX: used for testing, this method should never call exit() for a production release, since all logging is hidden away from the game engine dev
            AssertThreadAccess(const string& fp_FunctionName) //we don't require a lock since this method guarantees only one thread is operating on any data within the Logger instance
            const
        {
            if (this_thread::get_id() == pm_ThreadOwnerID)
            {
                return true;
            }

            stringstream f_UckCPlusPlus; //XXX: cpp is a dumb fucking language sometimes holy please make good features and not dumbass nonsense holy shit
            f_UckCPlusPlus << this_thread::get_id();
            string f_CallerThreadID = f_UckCPlusPlus.str();

            PrintError(format("Logger name: '{}' called method '{}' from the wrong thread, [Caller Thread ID]: {}", pm_LoggerName, fp_FunctionName, f_CallerThreadID));

            return false;
        }

        void
            ForceFlushAllLogs() //called by functions that already do an AssertThreadAccess call in them uwu this is to avoid double calling OwO!
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
    };
}