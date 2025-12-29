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

#include <thread>

#include "../Utils/RingBuffer.h"

//#ifndef $ //Used for bookkeeping and tracking what variables are mutable inside a function
//#define $
//#endif

constexpr uint32_t MAX_NUMBER_OF_LOGS = 1024;

constexpr const int FAILED_TO_CREATE_MAIN_WINDOW = -1000;
constexpr const int FAILED_TO_INITIALIZE_OPENGL = -1001;
constexpr const int FAILED_TO_INITIALIZE_VULKAN = -1002;

constexpr const int FATAL_SEGMENTATION_FAULT = -6969;

/// moody camel queue size uwu
constexpr const unsigned int MOODY_CAMEL_QUEUE_SIZE = 128;

namespace PeachCore {

    using namespace std;

    #if defined(_WIN32) || defined(_WIN64) && defined(PEACH_USING_OS_TERMINAL)

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
                cout << ("Was not able to set console mode to allow windows to display ANSI escape codes") << "\n";
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
        string Log;
        string Sender;
        uint8_t Level = 69;
    };

    //////////////////////////////////////////////
    // ThreadName Enum
    //////////////////////////////////////////////

    enum ThreadName : uint8_t
    {
        NoOwner = 0,
        MainThread = 1 << 0,
        RenderThread = 1 << 1,
        ResourceThread = 1 << 2,
        NetworkThread = 1 << 3,
        PhysicsThread = 1 << 4,
        AudioThread = 1 << 5,
        AllThreads = MainThread | RenderThread | ResourceThread | NetworkThread | PhysicsThread | AudioThread
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
            for (auto& _f : pm_LogFiles)
            {
                if (_f.second.is_open())
                {
                    _f.second.flush();
                }
            }  // Ensure all logs are flushed before destruction

            CloseOpenLogFiles(); //Closes any files that are open to prevent introducing vulnerabilities in privileged environments
        }

    //////////////////////////////////////////////
    // Public Constructor
    //////////////////////////////////////////////
    public:
        Logger() = default;

    ////////////////////////////////////////////////
    // Helper Enum For LogLevel Specification
    ////////////////////////////////////////////////
    public:
        enum LogLevel : uint8_t 
        {
            TRACE_LOG = 1 << 0,
            DEBUG_LOG = 1 << 1,
            INFO_LOG = 1 << 2,
            WARNING_LOG = 1 << 3,
            ERROR_LOG = 1 << 4,
            FATAL_LOG = 1 << 5,
            ALL_LOGS = TRACE_LOG | DEBUG_LOG | INFO_LOG | WARNING_LOG | ERROR_LOG | FATAL_LOG
        };

    //////////////////////////////////////////////
    // Protected Class Members
    //////////////////////////////////////////////
    protected:
        bool pm_HasBeenInitialized = false;

        unordered_map<string, ofstream> pm_LogFiles;

        unique_ptr<RingBuffer<LogMessage, MAX_NUMBER_OF_LOGS>> pm_LogSnapshotBuffer = nullptr;

        string pm_LoggerName = "No_Logger_Name";
        string pm_CurrentWorkingDirectory = "nothing";

        thread::id pm_ThreadOwnerID;
        ThreadName pm_ThreadOwnerName = ThreadName::NoOwner;

        uint8_t pm_ActiveLogMask = LogLevel::ALL_LOGS;
        LogLevel pm_FlushMask = static_cast<LogLevel>(LogLevel::ERROR_LOG | LogLevel::FATAL_LOG); // or make this user-configurable
        
        uint32_t pm_LogSizeCounter = 0;

    //////////////////////////////////////////////
    // Public Methods
    //////////////////////////////////////////////
    public:
        bool
            Initialize
            (
                const ThreadName fp_ThreadName,
                const string& fp_DesiredOutputDirectory,
                const string& fp_DesiredLoggerName,
                const uint8_t fp_LogLevelFlags,
                const bool fp_ShouldCreateOutputDirectory = true
            )
        {
#ifdef PEACH_DEBUG
            stringstream f_UckCPlusPlus; //XXX: cpp is a dumb fucking language sometimes holy please make good features and not dumbass nonsense holy shit
            f_UckCPlusPlus << this_thread::get_id();
            string f_CallerThreadID = f_UckCPlusPlus.str();

            //PrintError(format("Logger name: '{}' from thread number : {}, [Caller Thread ID]: {}", fp_DesiredLoggerName, static_cast<uint8_t>(fp_ThreadName), f_CallerThreadID));
#endif
            if (pm_HasBeenInitialized) //stops accidental reinitialization of logmanager
            {
                PrintError(format("Logger with name : '{}' has already been initialized, Logger is only allowed to initialize once", fp_DesiredLoggerName));
                return false;
            }

            pm_LoggerName = fp_DesiredLoggerName;

            pm_ActiveLogMask = fp_LogLevelFlags;

            pm_ThreadOwnerName = fp_ThreadName;
            pm_ThreadOwnerID = this_thread::get_id();

            pm_CurrentWorkingDirectory = fp_DesiredOutputDirectory + "/" + pm_LoggerName;

            // Ensure log directory exists
            if ((not filesystem::exists(pm_CurrentWorkingDirectory)) and fp_ShouldCreateOutputDirectory)
            {
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
            else if (not filesystem::exists(pm_CurrentWorkingDirectory))
            {
                PrintError("Failed to find valid log output directory");
                return false;
            }
            
            //Create Log files based off of log level flags
            const unordered_map<uint8_t, string> f_LogLevels = 
            {
                {TRACE_LOG, "trace"},
                {DEBUG_LOG, "debug"},
                {INFO_LOG, "info"},
                {WARNING_LOG, "warn"},
                {ERROR_LOG, "error"},
                {FATAL_LOG, "fatal"}
            };

            for (const auto& [__key, __val] : f_LogLevels)
            {
                if(pm_ActiveLogMask & __key)
                {
                    CreateLogFile(pm_CurrentWorkingDirectory, __val + ".log");
                }
            }

            pm_LogSnapshotBuffer = make_unique<RingBuffer<LogMessage, MAX_NUMBER_OF_LOGS>>();

            pm_HasBeenInitialized = true; //well if everything went as planned we should be good to set this to true uwu

            return true;
        }

        void
            UpdateThreadOwner()
        {
            pm_ThreadOwnerID = this_thread::get_id();
        }

        //////////////////// Flush All Logs ////////////////////

        void
            FlushAllLogs()
        {
            #ifdef PEACH_DEBUG
                if (not AssertThreadAccess("FlushAllLogs"))
                {
                    return;
                }
            #endif

            for (auto& _f : pm_LogFiles)
            {
                if (_f.second.is_open())
                {
                    _f.second.flush();
                }
            }
        }

        bool
            ValidateLogMsg(const uint8_t fp_LogLevel)
        {
            //return early without logging if loglevel isnt active or hasnt been initialized or if accessed from the wrong thread
            #ifdef PEACH_DEBUG
                return (pm_ActiveLogMask & fp_LogLevel) and pm_HasBeenInitialized and AssertThreadAccess("Log");
            #else
                    return (pm_ActiveLogMask & fp_LogLevel);
            #endif
        }

        void
            Trace
            (
                const string& fp_Message, 
                const string& fp_Sender
            )
        {
            if (ValidateLogMsg(LogLevel::TRACE_LOG))
            {
                const string f_TimeStamp = GetCurrentTimestamp();
                const string f_LogEntry = "[" + f_TimeStamp + "][trace][" + fp_Sender + "]: " + fp_Message;

                // Log to specific file and all-logs file
                const string f_LogFileName = "trace.log";

                if (pm_LogFiles.find(f_LogFileName) != pm_LogFiles.end() and pm_LogFiles[f_LogFileName].is_open())
                {
                    pm_LogFiles[f_LogFileName] << f_LogEntry << "\n";
                    // Print("size of ofstream: " + to_string(sizeof(pm_LogFiles[f_LogFileName])));
                    //pm_LogSizeCounter++;

                    //if (pm_LogFiles[f_LogFileName].tellg >= MAX_NUMBER_OF_LOGS)
                    //{
                    //    pm_LogFiles[f_LogFileName].flush();
                    //}
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
            if (ValidateLogMsg(LogLevel::DEBUG_LOG))
            {
                const string f_TimeStamp = GetCurrentTimestamp();
                const string f_LogEntry = "[" + f_TimeStamp + "][Debug][" + fp_Sender + "]: " + fp_Message;

                // Log to specific file and all-logs file
                const string f_LogFileName = "debug.log";

                if (pm_LogFiles.find(f_LogFileName) != pm_LogFiles.end() and pm_LogFiles[f_LogFileName].is_open())
                {
                    pm_LogFiles[f_LogFileName] << f_LogEntry << "\n";
                    // Print("size of ofstream: " + to_string(sizeof(pm_LogFiles[f_LogFileName])));
                    //pm_LogSizeCounter++;

                    //if (pm_LogFiles[f_LogFileName].tellg >= MAX_NUMBER_OF_LOGS)
                    //{
                    //    pm_LogFiles[f_LogFileName].flush();
                    //}
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
            if (ValidateLogMsg(LogLevel::INFO_LOG))
            {
                const string f_TimeStamp = GetCurrentTimestamp();
                const string f_LogEntry = "[" + f_TimeStamp + "][Info][" + fp_Sender + "]: " + fp_Message;

                // Log to specific file and all-logs file
                const string f_LogFileName = "info.log";

                if (pm_LogFiles.find(f_LogFileName) != pm_LogFiles.end() and pm_LogFiles[f_LogFileName].is_open())
                {
                    pm_LogFiles[f_LogFileName] << f_LogEntry << "\n";
                    // Print("size of ofstream: " + to_string(sizeof(pm_LogFiles[f_LogFileName])));
                    //pm_LogSizeCounter++;

                    //if (pm_LogFiles[f_LogFileName].tellg >= MAX_NUMBER_OF_LOGS)
                    //{
                    //    pm_LogFiles[f_LogFileName].flush();
                    //}
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
            if (ValidateLogMsg(LogLevel::WARNING_LOG))
            {
                const string f_TimeStamp = GetCurrentTimestamp();
                const string f_LogEntry = "[" + f_TimeStamp + "][Warning][" + fp_Sender + "]: " + fp_Message;

                // Log to specific file and all-logs file
                const string f_LogFileName = "warning.log";

                if (pm_LogFiles.find(f_LogFileName) != pm_LogFiles.end() and pm_LogFiles[f_LogFileName].is_open())
                {
                    pm_LogFiles[f_LogFileName] << f_LogEntry << "\n";
                    // Print("size of ofstream: " + to_string(sizeof(pm_LogFiles[f_LogFileName])));
                    //pm_LogSizeCounter++;

                    //if (pm_LogFiles[f_LogFileName].tellg >= MAX_NUMBER_OF_LOGS)
                    //{
                    //    pm_LogFiles[f_LogFileName].flush();
                    //}
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
            if (ValidateLogMsg(LogLevel::ERROR_LOG))
            {
                const string f_TimeStamp = GetCurrentTimestamp();
                const string f_LogEntry = "[" + f_TimeStamp + "][Error][" + fp_Sender + "]: " + fp_Message;

                // Log to specific file and all-logs file
                const string f_LogFileName = "error.log";

                if (pm_LogFiles.find(f_LogFileName) != pm_LogFiles.end() and pm_LogFiles[f_LogFileName].is_open())
                {
                    pm_LogFiles[f_LogFileName] << f_LogEntry << "\n";
                    // Print("size of ofstream: " + to_string(sizeof(pm_LogFiles[f_LogFileName])));
                    //pm_LogSizeCounter++;

                    //if (pm_LogFiles[f_LogFileName].tellg >= MAX_NUMBER_OF_LOGS)
                    //{
                    //    pm_LogFiles[f_LogFileName].flush();
                    //}
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
            if (ValidateLogMsg(LogLevel::FATAL_LOG))
            {
                const string f_TimeStamp = GetCurrentTimestamp();
                const string f_LogEntry = "[" + f_TimeStamp + "][Fatal][" + fp_Sender + "]: " + fp_Message;

                // Log to specific file and all-logs file
                const string f_LogFileName = "fatal.log";

                if (pm_LogFiles.find(f_LogFileName) != pm_LogFiles.end() and pm_LogFiles[f_LogFileName].is_open())
                {
                    pm_LogFiles[f_LogFileName] << f_LogEntry << "\n";
                    // Print("size of ofstream: " + to_string(sizeof(pm_LogFiles[f_LogFileName])));
                    //pm_LogSizeCounter++;

                    //if (pm_LogFiles[f_LogFileName].tellg >= MAX_NUMBER_OF_LOGS)
                    //{
                    //    pm_LogFiles[f_LogFileName].flush();
                    //}
                }

                #ifdef PEACH_USING_OS_TERMINAL
                    PrintError(f_LogEntry, Colours::Magenta);
                #endif
            }
        }

        //////////////////// Logging Functions  ////////////////////

        //string
        //    Log
        //    (
        //        const string& fp_Message,
        //        const string& fp_Sender,
        //        const string& fp_LogLevel
        //    )
        //{
        //    const string f_TimeStamp = GetCurrentTimestamp();
        //    const string f_LogEntry = "[" + f_TimeStamp + "]["+ fp_LogLevel +"][" + fp_Sender + "]: " + fp_Message;

        //    // Log to specific file and all-logs file
        //    const string f_LogFileName = fp_LogLevel + ".log";

        //    if (pm_LogFiles.find(f_LogFileName) != pm_LogFiles.end() and pm_LogFiles[f_LogFileName].is_open())
        //    {
        //        pm_LogFiles[f_LogFileName] << f_LogEntry << "\n";
        //        // Print("size of ofstream: " + to_string(sizeof(pm_LogFiles[f_LogFileName])));
        //        //pm_LogSizeCounter++;

        //        //if (pm_LogFiles[f_LogFileName].tellg >= MAX_NUMBER_OF_LOGS)
        //        //{
        //        //    pm_LogFiles[f_LogFileName].flush();
        //        //}
        //    }

        //    return f_LogEntry;
        //}

    //////////////////////////////////////////////
    // Protected Methods
    //////////////////////////////////////////////
    protected:
        //////////////////// Utility Functions  ////////////////////

        void
            CreateLogFile
            (
                const string& fp_FilePath,
                const string& fp_FileName
            )
        {
            ofstream f_LogFile;

            f_LogFile.open(fp_FilePath + "/" + fp_FileName, ios::out | ios::app);

            if (not f_LogFile.is_open())
            {
                PrintError(format("Failed to open log file: '{}'", fp_FileName));
            }
            else
            {
                pm_LogFiles[fp_FileName] = move(f_LogFile);
            }
        }

        [[nodiscard]] inline string //thank you chat-gpt uwu
            GetCurrentTimestamp()
            const noexcept
        {
            const auto now = chrono::system_clock::now();
            auto time_t_now = chrono::system_clock::to_time_t(now);
           
            tm local_time{};

            #if defined(_WIN32) || defined(_WIN64) //needa do this since localtime() isnt threadsafe uwu
                localtime_s(&local_time, &time_t_now);
            #else
                localtime_r(&time_t_now, &local_time);
            #endif

            stringstream ss;
            ss << put_time(&local_time, "%Y-%m-%d %H:%M:%S");

            const auto since_epoch = now.time_since_epoch();
            const auto milliseconds = chrono::duration_cast<chrono::milliseconds>(since_epoch).count() % 1000;

            ss << '.' << setfill('0') << setw(3) << milliseconds;

            return ss.str();
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

        #ifdef PEACH_DEBUG
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
        #endif
    };
}