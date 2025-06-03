/*******************************************************************
 *                                             Peach-E v0.0.1
 *                           Created by Ranyodh Mandur - 🍑 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                     Peach-E is a free open source game engine
********************************************************************/
#pragma once

#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>

#include <chrono>
#include <iomanip>
#include <sstream>

#include <map>
#include <format>

#include <thread>

#include "../Utils/RingBuffer.h"

//#ifndef $ //Used for bookkeeping and tracking what variables are mutable inside a function
//#define $
//#endif

using namespace std;

#if defined(_WIN32) || defined(_WIN64)

    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN

    #include <windows.h>

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

constexpr uint32_t MAX_NUMBER_OF_LOGS = 1024;

constexpr const int FAILED_TO_CREATE_MAIN_WINDOW = -1000;
constexpr const int FAILED_TO_INITIALIZE_OPENGL = -1001;
constexpr const int FAILED_TO_INITIALIZE_VULKAN = -1002;

constexpr const int FATAL_SEGMENTATION_FAULT = -6969;

#ifdef PEACH_DEBUG
    #define PEACH_LOG LogAndPrint
#else
    #define PEACH_LOG Log
#endif

namespace PeachCore {

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
    // Console Struct
    //////////////////////////////////////////////

    struct Console
    {
    public:
        Console()
        {
            pm_MainThreadLogSnapshotBuffer = make_unique<RingBuffer<LogMessage, MAX_NUMBER_OF_LOGS>>();
            pm_RenderThreadLogSnapshotBuffer = make_unique<RingBuffer<LogMessage, MAX_NUMBER_OF_LOGS>>();
            pm_AudioThreadLogSnapshotBuffer = make_unique<RingBuffer<LogMessage, MAX_NUMBER_OF_LOGS>>();
            pm_ResourceThreadLogSnapshotBuffer = make_unique<RingBuffer<LogMessage, MAX_NUMBER_OF_LOGS>>();
            pm_PhysicsThreadLogSnapshotBuffer = make_unique<RingBuffer<LogMessage, MAX_NUMBER_OF_LOGS>>();
            pm_NetworkThreadLogSnapshotBuffer = make_unique<RingBuffer<LogMessage, MAX_NUMBER_OF_LOGS>>();
        }

        ~Console() //idk i think windows heap cleanup is more efficient but whatever this feels better uwu
        {
            //pm_MainThreadLogSnapshotBuffer.clear();
            //pm_RenderThreadLogSnapshotBuffer.clear();
            //pm_AudioThreadLogSnapshotBuffer.clear();
            //pm_ResourceThreadLogBuffer.clear();
            //pm_PhysicsThreadLogBuffer.clear();
            //pm_NetworkThreadLogBuffer.clear();
        }

        void
            ClearConsoleBuffer
            (
                const uint8_t fp_DesiredTab
            )
        {

        }

        //WIP NEED TO LOCK THE THREAD SO THAT WE CAN SAFELY QUERY THE LOG BUFFERS SINCE THEY CAN BE WRITTEN TOO WHILE
        bool
            QueryLogBufferByLevel
            (
                const ThreadName fp_NameOfLogBuffer,
                const uint8_t fp_DesiredLogLevelQuery
            )
        {
            //switch (fp_NameOfLogBuffer)
            //{
            //    case ThreadName::MainThread: pm_MainThreadLogSnapshotBuffer.emplace_back(fp_Message, fp_Sender); break;
            //    case ThreadName::RenderThread: pm_RenderThreadLogSnapshotBuffer.emplace_back(fp_Message, fp_Sender); break;
            //    case ThreadName::AudioThread: pm_AudioThreadLogSnapshotBuffer.emplace_back(fp_Message, fp_Sender); break;
            //    case ThreadName::ResourceThread: pm_ResourceThreadLogBuffer.emplace_back(fp_Message, fp_Sender); break;
            //    case ThreadName::PhysicsThread: pm_PhysicsThreadLogBuffer.emplace_back(fp_Message, fp_Sender); break;
            //    case ThreadName::NetworkThread: pm_NetworkThreadLogBuffer.emplace_back(fp_Message, fp_Sender); break;
            //    default:
            //        PrintError("Attempted to Log to an invalid thread log buffer: Did you check for any typos when calling the Log() function?\n\tSender: " + fp_Sender + "\n\tMessage: " + fp_Message);
            //}

            return true;
        }

        void
            AddLog
            (
                const string& fp_Message,
                const string& fp_Sender,
                const uint8_t fp_LogLevel,
                const ThreadName fp_ThreadName
            )
        {
            switch (fp_ThreadName)
            {
                case ThreadName::MainThread: pm_MainThreadLogSnapshotBuffer->ForceEmplace(fp_Message, fp_Sender, fp_LogLevel); break;
                case ThreadName::RenderThread: pm_RenderThreadLogSnapshotBuffer->ForceEmplace(fp_Message, fp_Sender, fp_LogLevel); break;
                case ThreadName::AudioThread: pm_AudioThreadLogSnapshotBuffer->ForceEmplace(fp_Message, fp_Sender, fp_LogLevel); break;
                case ThreadName::ResourceThread: pm_ResourceThreadLogSnapshotBuffer->ForceEmplace(fp_Message, fp_Sender, fp_LogLevel); break;
                case ThreadName::PhysicsThread: pm_PhysicsThreadLogSnapshotBuffer->ForceEmplace(fp_Message, fp_Sender, fp_LogLevel); break;
                case ThreadName::NetworkThread: pm_NetworkThreadLogSnapshotBuffer->ForceEmplace(fp_Message, fp_Sender, fp_LogLevel); break;
                default:
                    PrintError("Attempted to Log to an invalid thread log buffer: Did you check for any typos when calling the Log() function?\n\tSender: " + fp_Sender + "\n\tMessage: " + fp_Message);
            }
        }

    private:
        //logs are all related to the current project game logs

        unique_ptr<RingBuffer<LogMessage, MAX_NUMBER_OF_LOGS>> pm_MainThreadLogSnapshotBuffer = nullptr;
        unique_ptr<RingBuffer<LogMessage, MAX_NUMBER_OF_LOGS>> pm_RenderThreadLogSnapshotBuffer = nullptr;
        unique_ptr<RingBuffer<LogMessage, MAX_NUMBER_OF_LOGS>> pm_AudioThreadLogSnapshotBuffer = nullptr;
        unique_ptr<RingBuffer<LogMessage, MAX_NUMBER_OF_LOGS>> pm_ResourceThreadLogSnapshotBuffer = nullptr;
        unique_ptr<RingBuffer<LogMessage, MAX_NUMBER_OF_LOGS>> pm_PhysicsThreadLogSnapshotBuffer = nullptr;
        unique_ptr<RingBuffer<LogMessage, MAX_NUMBER_OF_LOGS>> pm_NetworkThreadLogSnapshotBuffer = nullptr;

        //mutex pm_ConsoleMutex;
    };

    //////////////////////////////////////////////
    // LogManager Class
    //////////////////////////////////////////////

    class LogManager
    {
    //////////////////////////////////////////////
    // Public Destructor
    //////////////////////////////////////////////
    public:
        ~LogManager() ///XXX: Just copy and pasted the flushalllogs method because they have the assert at the beginning and wont work with premature exit
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
        LogManager() = default;

    ////////////////////////////////////////////////
    // Helper Enum For LogLevel Specification
    ////////////////////////////////////////////////
    public:
        enum LogLevel : uint8_t 
        {
            Trace = 1 << 0,
            Debug = 1 << 1,
            Info = 1 << 2,
            Warning = 1 << 3,
            Error = 1 << 4,
            Fatal = 1 << 5,
            All = Trace | Debug | Info | Warning | Error | Fatal
        };

    //////////////////////////////////////////////
    // Protected Class Members
    //////////////////////////////////////////////
    protected:
        bool pm_HasBeenInitialized = false;

        map<string, ofstream> pm_LogFiles;

        string pm_LoggerName = "No_Logger_Name";
        string pm_CurrentWorkingDirectory = "nothing";

        shared_ptr<Console> pm_Console = nullptr;

        thread::id pm_ThreadOwnerID;
        ThreadName pm_ThreadOwnerName = ThreadName::NoOwner;

        LogLevel pm_ActiveLogMask = LogLevel::All;
        LogLevel pm_FlushMask = static_cast<LogLevel>(LogLevel::Error | LogLevel::Fatal); // or make this user-configurable
        
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
                shared_ptr<Console> fp_Console,
                const uint8_t fp_LogLevelFlags,
                const bool fp_ShouldCreateOutputDirectory = true
            )
        {
            if (pm_HasBeenInitialized) //stops accidental reinitialization of logmanager
            {
                PrintError("LogManager has already been initialized, LogManager is only allowed to initialize once per run");
                return false;
            }

            pm_LoggerName = fp_DesiredLoggerName;
            pm_Console = fp_Console;

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
            const map<uint8_t, string> f_LogLevels = 
            {
                {Trace, "trace"},
                {Debug, "debug"},
                {Info, "info"},
                {Warning, "warn"},
                {Error, "error"},
                {Fatal, "fatal"}
            };

            for (const auto& [__key, __val] : f_LogLevels)
            {
                if(fp_LogLevelFlags & __key)
                {
                    CreateLogFile(pm_CurrentWorkingDirectory, __val + ".log");
                }
            }

            pm_HasBeenInitialized = true; //well if everything went as planned we should be good to set this to true uwu

            return true;
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

        //////////////////// Logging Functions  ////////////////////

        void
            Log
            (
                const string& fp_Message,
                const string& fp_Sender,
                const uint8_t fp_LogLevel
            )
        {
            //return early without logging if loglevel isnt active or hasnt been initialized or if accessed from the wrong thread
            #ifdef PEACH_DEBUG
                if (not ((pm_ActiveLogMask & fp_LogLevel) or pm_HasBeenInitialized or AssertThreadAccess("Log"))) return;
            #else
                if (not ((pm_ActiveLogMask & fp_LogLevel) or pm_HasBeenInitialized)) return;
            #endif

            string f_LogLevel;

            switch (fp_LogLevel)
            {
                case LogLevel::Trace:
                    f_LogLevel = "trace";
                    break;
                case LogLevel::Debug:
                    f_LogLevel = "debug";
                    break;
                case LogLevel::Info:
                    f_LogLevel = "info";
                    break;
                case LogLevel::Warning:
                    f_LogLevel = "warn";
                    break;
                case LogLevel::Error:
                    f_LogLevel = "error"; //not bright oooo soo dark and moody and complex and hard to reach and engage with ><
                    break;
                case LogLevel::Fatal:
                    f_LogLevel = "fatal";
                    break;
                default:
                    PrintError(Log("Did not input a valid option for log level in Log()", pm_LoggerName, "error", fp_LogLevel));
                    Print(Log(fp_Message, fp_Sender, "error", fp_LogLevel));
                    return;
            }

            const string f_TimeStamp = GetCurrentTimestamp();
            const string f_LogEntry = "[" + f_TimeStamp + "]" + "[" + f_LogLevel + "]" + "[" + fp_Sender + "]: " + fp_Message + "\n";

            // Log to specific file and all-logs file
            const string f_LogFileName = f_LogLevel + ".log";

            if (pm_LogFiles.find(f_LogFileName) != pm_LogFiles.end() and pm_LogFiles[f_LogFileName].is_open())
            {
                pm_LogFiles[f_LogFileName] << f_LogEntry;
                // Print("size of ofstream: " + to_string(sizeof(pm_LogFiles[f_LogFileName])));
                //pm_LogSizeCounter++;

                //if (pm_LogFiles[f_LogFileName].tellg >= MAX_NUMBER_OF_LOGS)
                //{
                //    pm_LogFiles[f_LogFileName].flush();
                //}
            }

            pm_Console->AddLog(format("[{}][{}]: {}", f_LogLevel, pm_LoggerName, fp_Message), pm_LoggerName, fp_LogLevel, pm_ThreadOwnerName);
        }

        void
            Logf
            (
                const string& fp_Message,
                const string& fp_Sender,
                const uint8_t fp_LogLevel
            )
        {
            //return early without logging if loglevel isnt active or hasnt been initialized or if accessed from the wrong thread
            #ifdef PEACH_DEBUG
                if (not ((pm_ActiveLogMask & fp_LogLevel) or pm_HasBeenInitialized or AssertThreadAccess("Log"))) return;
            #else
                if (not ((pm_ActiveLogMask & fp_LogLevel) or pm_HasBeenInitialized)) return;
            #endif

            string f_LogLevel;

            switch (fp_LogLevel)
            {
                case LogLevel::Trace:
                    f_LogLevel = "trace";
                    break;
                case LogLevel::Debug:
                    f_LogLevel = "debug";
                    break;
                case LogLevel::Info:
                    f_LogLevel = "info";
                    break;
                case LogLevel::Warning:
                    f_LogLevel = "warn";
                    break;
                case LogLevel::Error:
                    f_LogLevel = "error"; //not bright oooo soo dark and moody and complex and hard to reach and engage with ><
                    break;
                case LogLevel::Fatal:
                    f_LogLevel = "fatal";
                    break;
                default:
                    PrintError(Log("Did not input a valid option for log level in Log()", pm_LoggerName, "error", fp_LogLevel));
                    Print(Log(fp_Message, fp_Sender, "error", fp_LogLevel));
                    return;
            }

            const string f_TimeStamp = GetCurrentTimestamp();
            const string f_LogEntry = "[" + f_TimeStamp + "]" + "[" + f_LogLevel + "]" + "[" + fp_Sender + "]: " + fp_Message + "\n";

            // Log to specific file and all-logs file
            const string f_LogFileName = f_LogLevel + ".log";

            if (pm_LogFiles.find(f_LogFileName) != pm_LogFiles.end() and pm_LogFiles[f_LogFileName].is_open())
            {
                pm_LogFiles[f_LogFileName] << f_LogEntry;
                // Print("size of ofstream: " + to_string(sizeof(pm_LogFiles[f_LogFileName])));
                //pm_LogSizeCounter++;

                //if (pm_LogFiles[f_LogFileName].tellg >= MAX_NUMBER_OF_LOGS)
                //{
                //    pm_LogFiles[f_LogFileName].flush();
                //}
            }

            pm_Console->AddLog(format("[{}][{}]: {}", f_LogLevel, pm_LoggerName, fp_Message), pm_LoggerName, fp_LogLevel, pm_ThreadOwnerName);
        }

        void
            LogAndPrint
            (
                const string& fp_Message,
                const string& fp_Sender,
                const uint8_t fp_LogLevel
            )
        {
            //return early without logging if loglevel isnt active or hasnt been initialized or if accessed from the wrong thread
            #ifdef PEACH_DEBUG
                if (not ((pm_ActiveLogMask & fp_LogLevel) or pm_HasBeenInitialized or AssertThreadAccess("LogAndPrint"))) return;
            #else
                if (not ((pm_ActiveLogMask & fp_LogLevel) or pm_HasBeenInitialized)) return;
            #endif

            // Log to console

            switch (fp_LogLevel)
            {
                case LogLevel::Trace:
                    Print(Log(fp_Message, fp_Sender, "trace", fp_LogLevel), Colours::BrightWhite);
                    break;
                case LogLevel::Debug:
                    Print(Log(fp_Message, fp_Sender, "debug", fp_LogLevel), Colours::BrightBlue);
                    break;
                case LogLevel::Info:
                    Print(Log(fp_Message, fp_Sender, "info", fp_LogLevel), Colours::BrightGreen);
                    break;
                case LogLevel::Warning:
                    Print(Log(fp_Message, fp_Sender, "warn", fp_LogLevel), Colours::BrightYellow);
                    break;
                case LogLevel::Error:
                    PrintError(Log(fp_Message, fp_Sender, "error", fp_LogLevel), Colours::Red); //not bright oooo soo dark and moody and complex and hard to reach and engage with ><
                    break;
                case LogLevel::Fatal:
                    PrintError(Log(fp_Message, fp_Sender, "fatal", fp_LogLevel), Colours::BrightMagenta);
                    break;
                default:
                    PrintError(Log("Did not input a valid option for log level in LogAndPrint()", pm_LoggerName, "error", fp_LogLevel));
                    Print(Log(fp_Message, fp_Sender, "error", fp_LogLevel));
            }
        }

    //////////////////////////////////////////////
    // Protected Methods
    //////////////////////////////////////////////
    protected:

        //////////////////// Protected Logging Function  ////////////////////
        //XXX: this is protected since it's supposed to be only called from LogAndPrint()
        string
            Log
            (
                const string& fp_Message,
                const string& fp_Sender,
                const string& fp_LogLevel,
                const uint8_t fp_EnumLogLevel //used for adding a log to console
            )
        {
            const string f_TimeStamp = GetCurrentTimestamp();
            string f_LogEntry = "[" + f_TimeStamp + "]" + "[" + fp_LogLevel + "]" + "[" + fp_Sender + "]: " + fp_Message + "\n";

            // Log to specific file and all-logs file
            const string f_LogFileName = fp_LogLevel + ".log";

            if (pm_LogFiles.find(f_LogFileName) != pm_LogFiles.end() and pm_LogFiles[f_LogFileName].is_open())
            {
                pm_LogFiles[f_LogFileName] << f_LogEntry;
            }

            pm_Console->AddLog(format("[{}][{}]: {}", fp_LogLevel, pm_LoggerName, fp_Message), pm_LoggerName, fp_EnumLogLevel, pm_ThreadOwnerName);

            pm_LogSizeCounter++;

            return f_LogEntry;
        }

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
                AssertThreadAccess(const string& fp_FunctionName) //we don't require a lock since this method guarantees only one thread is operating on any data within the LogManager instance
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