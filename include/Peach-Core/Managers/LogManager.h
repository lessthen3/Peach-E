/*******************************************************************
 *                                             Peach-E v0.0.1
 *                           Created by Ranyodh Mandur - 🍑 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                         Peach-E is an open-source game engine
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

constexpr const int FAILED_TO_CREATE_MAIN_WINDOW = -1000;
constexpr const int FAILED_TO_INITIALIZE_OPENGL = -1001;
constexpr const int FAILED_TO_INITIALIZE_VULKAN = -1002;

constexpr const int FATAL_SEGMENTATION_FAULT = -6969;

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
        string m_Log;
        string m_Sender;

        LogMessage(const string& fp_Log, const string& fp_Sender)
        {
            m_Log = fp_Log;
            m_Sender = fp_Sender;
        }
    };

    //////////////////////////////////////////////
    // Console Struct
    //////////////////////////////////////////////

    struct Console
    {
    public:
        Console() //should heap alloc these since they can get large and me no want stack overflow uwu xxdxdxdxd rawr so random
        {
            pm_MainThreadLogBuffer = make_unique< vector<LogMessage> >();
            pm_RenderThreadLogBuffer = make_unique< vector<LogMessage> >();
            pm_AudioThreadLogBuffer = make_unique< vector<LogMessage> >();
            pm_ResourceThreadLogBuffer = make_unique< vector<LogMessage> >();
            pm_PhysicsThreadLogBuffer = make_unique< vector<LogMessage> >();
            pm_NetworkThreadLogBuffer = make_unique< vector<LogMessage> >();
        }

        ~Console() //idk i think windows heap cleanup is more efficient but whatever this feels better uwu
        {
            pm_MainThreadLogBuffer.reset(nullptr);
            pm_RenderThreadLogBuffer.reset(nullptr);
            pm_AudioThreadLogBuffer.reset(nullptr);
            pm_ResourceThreadLogBuffer.reset(nullptr);
            pm_PhysicsThreadLogBuffer.reset(nullptr);
            pm_NetworkThreadLogBuffer.reset(nullptr);
        }

        void
            ClearConsoleBuffer
            (
                const string& fp_DesiredTab
            )
        {

        }

        //WIP NEED TO LOCK THE THREAD SO THAT WE CAN SAFELY QUERY THE LOG BUFFERS SINCE THEY CAN BE WRITTEN TOO WHILE
        vector<string>
            QueryLogBufferByLevel
            (
                const string& fp_DesiredLogLevelQuery,
                const string& fp_NameOfLogBuffer
            )
        {
            //if (fp_NameOfLogBuffer == "main_thread")
            //{
            //    pm_MainThreadLogBuffer->push_back({ fp_Message, fp_Sender });
            //}
            //else if (fp_NameOfLogBuffer == "render_thread")
            //{
            //    pm_RenderThreadLogBuffer->push_back({ fp_Message, fp_Sender });
            //}
            //else if (fp_NameOfLogBuffer == "audio_thread")
            //{
            //    pm_AudioThreadLogBuffer->push_back({ fp_Message, fp_Sender });
            //}
            //else if (fp_NameOfLogBuffer == "resource_thread")
            //{
            //    pm_ResourceThreadLogBuffer->push_back({ fp_Message, fp_Sender });
            //}
            //else if (fp_NameOfLogBuffer == "physics_thread")
            //{
            //    pm_PhysicsThreadLogBuffer->push_back({ fp_Message, fp_Sender });
            //}
            //else if (fp_NameOfLogBuffer == "network_thread")
            //{
            //    pm_NetworkThreadLogBuffer->push_back({ fp_Message, fp_Sender });
            //}
            //else
            //{
            //    PrintError("Attempted to Log to an invalid thread log buffer: Did you check for any typos when calling the Log() function?\n\tSender: " + fp_Sender + "\n\tMessage: " + fp_Message);
            //}
        }

        void
            AddLog
            (
                const string& fp_Message,
                const string& fp_Sender,
                const string& fp_ThreadName
            )
        {
            if (fp_ThreadName == "main_thread")
            {
                pm_MainThreadLogBuffer->emplace_back(fp_Message, fp_Sender);
            }
            else if (fp_ThreadName == "render_thread")
            {
                pm_RenderThreadLogBuffer->emplace_back(fp_Message, fp_Sender);
            }
            else if (fp_ThreadName == "audio_thread")
            {
                pm_AudioThreadLogBuffer->emplace_back(fp_Message, fp_Sender);
            }
            else if (fp_ThreadName == "resource_thread")
            {
                pm_ResourceThreadLogBuffer->emplace_back(fp_Message, fp_Sender);
            }
            else if (fp_ThreadName == "physics_thread")
            {
                pm_PhysicsThreadLogBuffer->emplace_back(fp_Message, fp_Sender);
            }
            else if (fp_ThreadName == "network_thread")
            {
                pm_NetworkThreadLogBuffer->emplace_back(fp_Message, fp_Sender);
            }
            else
            {
                PrintError("Attempted to Log to an invalid thread log buffer: Did you check for any typos when calling the Log() function?\n\tSender: " + fp_Sender + "\n\tMessage: " + fp_Message);
            }
        }

    private:

        unique_ptr<vector<LogMessage>> pm_MainThreadLogBuffer = nullptr;
        unique_ptr<vector<LogMessage>> pm_RenderThreadLogBuffer = nullptr;
        unique_ptr<vector<LogMessage>> pm_AudioThreadLogBuffer = nullptr;
        unique_ptr<vector<LogMessage>> pm_ResourceThreadLogBuffer = nullptr;
        unique_ptr<vector<LogMessage>> pm_PhysicsThreadLogBuffer = nullptr;
        unique_ptr<vector<LogMessage>> pm_NetworkThreadLogBuffer = nullptr;


        //logs are all related to the current project game logs
        //map<const string, vector<string>> pm_Buffers = 
        //{
        //    {"everything", vector<string>()},

        //    {"editor_warn", vector<string>()},
        //    {"editor_error", vector<string>()}
        //};
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
        enum class LogLevel : int
        {
            Trace,
            Debug,
            Info,
            Warning,
            Error,
            Fatal
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
        string pm_ThreadOwnerName;

    //////////////////////////////////////////////
    // Public Methods
    //////////////////////////////////////////////
    public:
        bool
            Initialize
            (
                const string& fp_ThreadName,
                const string& fp_DesiredOutputDirectory,
                const string& fp_DesiredLoggerName,
                shared_ptr<Console> fp_Console,
                const string& fp_MinLogLevel = "trace",
                const string& fp_MaxLogLevel = "fatal"
            )
        {
            if (pm_HasBeenInitialized) //stops accidental reinitialization of logmanager
            {
                PrintError("LogManager has already been initialized, LogManager is only allowed to initialize once per run");
                return false;
            }

            pm_ThreadOwnerName = fp_ThreadName;
            pm_ThreadOwnerID = this_thread::get_id();

            pm_CurrentWorkingDirectory = fp_DesiredOutputDirectory + "/" + fp_DesiredLoggerName;

            // Ensure log directory exists
            if (not filesystem::exists(pm_CurrentWorkingDirectory))
            {
                try
                {
                    filesystem::create_directories(pm_CurrentWorkingDirectory);
                }
                catch (const exception& f_Exception)
                {
                    PrintError(format("An exception was thrown inside LogManager: {}", f_Exception.what()));
                    return false;
                }
            }

            if (fp_MinLogLevel == fp_MaxLogLevel)
            {
                CreateLogFile(pm_CurrentWorkingDirectory, fp_MinLogLevel + ".log"); //could be min or max just chose min cause y not
            }
            else
            {
                const vector<string> f_LogLevels = { "trace", "debug", "info", "warn", "error", "fatal", "all-logs" };
                bool f_ShouldInclude = false;

                for (const auto& _level : f_LogLevels)
                {
                    if (_level == fp_MinLogLevel)
                    {
                        f_ShouldInclude = true;
                    }
                    else if (_level == fp_MaxLogLevel and fp_MaxLogLevel != "fatal")
                    {
                        f_ShouldInclude = false;
                    }

                    if(f_ShouldInclude or _level == "all-logs")
                    {
                        CreateLogFile(pm_CurrentWorkingDirectory, _level + ".log");
                    }
                }
            }

            pm_LoggerName = fp_DesiredLoggerName;
            pm_Console = fp_Console;

            pm_HasBeenInitialized = true; //well if everything went as planned we should be good to set this to true uwu

            return true;
        }

        bool
            Initialize
            (
                const string& fp_ThreadName,
                const string& fp_DesiredOutputDirectory,
                const string& fp_DesiredLoggerName,
                shared_ptr<Console> fp_Console,
                const vector<string>& fp_DesiredLogLevels
            )
        {
            if (fp_DesiredLogLevels.size() == 0)
            {
                PrintError("LogManager tried to initialize with no value for specific log filtering");
                return false;
            }

            if (pm_HasBeenInitialized) //stops accidental reinitialization of logmanager
            {
                PrintError("LogManager has already been initialized, LogManager is only allowed to initialize once per run");
                return false;
            }

            pm_ThreadOwnerName = fp_ThreadName;
            pm_ThreadOwnerID = this_thread::get_id();

            pm_CurrentWorkingDirectory = fp_DesiredOutputDirectory + "/" + fp_DesiredLoggerName;

            // Ensure log directory exists
            if (not filesystem::exists(pm_CurrentWorkingDirectory))
            {
                try
                {
                    filesystem::create_directories(pm_CurrentWorkingDirectory);
                }
                catch (const exception& ex)
                {
                    PrintError(format("An exception was thrown inside LogManager: {}", ex.what()));
                    return false;
                }
            }

            const vector<string> f_AllowedLogLevels = { "trace", "debug", "info", "warn", "error", "fatal", "all-logs" };

            for (const auto& _level : fp_DesiredLogLevels)
            {
                CreateLogFile(pm_CurrentWorkingDirectory, _level + ".log");

                if (count(f_AllowedLogLevels.begin(), f_AllowedLogLevels.end(), _level) == 0)
                {
                    PrintError("Invalid log level was input when filtering for individual log files");
                    return false;
                }
            }

            pm_Console = fp_Console;
            pm_HasBeenInitialized = true; //well if everything went as planned we should be good to set this to true uwu

            return true;
        }

        inline void ///XXX: used for testing, this method should never call exit() for a production release, since all logging is hidden away from the game engine dev
            AssertThreadAccess(const string& fp_FunctionName) //we don't require a lock since this method guarantees only one thread is operating on any data within the LogManager instance
            const
        {
            if (this_thread::get_id() != pm_ThreadOwnerID)
            {
                stringstream f_UckCPP; //XXX: cpp is a dumb fucking language sometimes holy please make good features and not dumbass nonsense holy shit
                f_UckCPP << this_thread::get_id();
                string f_CallerThreadID = f_UckCPP.str();

                PrintError(format("LogManager method '{}' called from the wrong thread, [Caller Thread ID]: {}. Exiting...", fp_FunctionName, f_CallerThreadID));
                
                exit(-69);
            }
        }

        //////////////////// Flush All Logs ////////////////////

        void
            FlushAllLogs()
        {
            AssertThreadAccess("FlushAllLogs");

            for (auto& _f : pm_LogFiles)
            {
                if (_f.second.is_open())
                {
                    _f.second.flush();
                }
            }
        }

        //////////////////// Logging Functions  ////////////////////

        string
            Log
            (
                const string& fp_Message,
                const string& fp_Sender,
                const string& fp_LogLevel
            )
        {
            AssertThreadAccess("Log");

            string f_TimeStamp = GetCurrentTimestamp();
            string f_LogEntry = "[" + f_TimeStamp + "]" + "[" + fp_LogLevel + "]" + "[" + fp_Sender + "]: " + fp_Message + "\n";

            // Log to specific file and all-logs file
            const string f_LogFileName = fp_LogLevel + ".log";
            const string f_AllLogsName = "all-logs.log";

            if (pm_LogFiles.find(f_LogFileName) != pm_LogFiles.end() and pm_LogFiles[f_LogFileName].is_open())
            {
                pm_LogFiles[f_LogFileName] << f_LogEntry;
            }

            if (pm_LogFiles.find(f_AllLogsName) != pm_LogFiles.end() and pm_LogFiles[f_AllLogsName].is_open())
            {
                pm_LogFiles[f_AllLogsName] << f_LogEntry;
            }

            pm_Console->AddLog(format("[{}][{}]: {} \n", fp_LogLevel, pm_LoggerName, fp_Message), pm_LoggerName, pm_ThreadOwnerName);

            return f_LogEntry;
        }

        string
            LogNotThreadSafe ///XXX: pretty much just another Log function copy without the assert, i just wanted the new name for being explicit
            (
                const string& fp_Message,
                const string& fp_Sender,
                const string& fp_LogLevel
            )
        {
            string f_TimeStamp = GetCurrentTimestamp();
            string f_LogEntry = "[" + f_TimeStamp + "]" + "[" + fp_LogLevel + "]" + "[" + fp_Sender + "]: " + fp_Message + "\n";

            // Log to specific file and all-logs file
            const string f_LogFileName = fp_LogLevel + ".log";
            const string f_AllLogsName = "all-logs.log";

            if (pm_LogFiles.find(f_LogFileName) != pm_LogFiles.end() and pm_LogFiles[f_LogFileName].is_open())
            {
                pm_LogFiles[f_LogFileName] << f_LogEntry;
            }

            if (pm_LogFiles.find(f_AllLogsName) != pm_LogFiles.end() and pm_LogFiles[f_AllLogsName].is_open())
            {
                pm_LogFiles[f_AllLogsName] << f_LogEntry;
            }

            pm_Console->AddLog(format("[{}][{}]: {} \n", fp_LogLevel, pm_LoggerName, fp_Message), pm_LoggerName, pm_ThreadOwnerName);

            return f_LogEntry;
        }

        void
            LogAndPrint
            (
                const string& fp_Message,
                const string& fp_Sender,
                const LogLevel fp_LogLevel
            )
        {
            // Log to console

            switch (fp_LogLevel)
            {
                case LogLevel::Trace:
                    Print(Log(fp_Message, fp_Sender, "trace"), Colours::BrightWhite);
                    break;
                case LogLevel::Debug:
                    Print(Log(fp_Message, fp_Sender, "debug"), Colours::BrightBlue);
                    break;
                case LogLevel::Info:
                    Print(Log(fp_Message, fp_Sender, "info"), Colours::BrightGreen);
                    break;
                case LogLevel::Warning:
                    Print(Log(fp_Message, fp_Sender, "warn"), Colours::BrightYellow);
                    break;
                case LogLevel::Error:
                    PrintError(Log(fp_Message, fp_Sender, "error"), Colours::Red); //not bright oooo soo dark and moody and complex and hard to reach and engage with ><
                    break;
                case LogLevel::Fatal:
                    PrintError(Log(fp_Message, fp_Sender, "fatal"), Colours::BrightMagenta);
                    break;
                default:
                    PrintError(Log("Did not input a valid option for log level in LogAndPrint()", "LogManager", "error"));
                    Print(Log(fp_Message, fp_Sender, "error"));
            }
        }

    //////////////////////////////////////////////
    // Protected Methods
    //////////////////////////////////////////////
    protected:
        void
            CreateLogFile
            (
                const string& fp_FilePath,
                const string& fp_FileName
            )
        {
            ofstream f_File;

            f_File.open(fp_FilePath + "/" + fp_FileName, ios::out | ios::app);

            if (not f_File.is_open())
            {
                PrintError(format("Failed to open log file: '{}'", fp_FileName));
            }
            else
            {
                pm_LogFiles[fp_FileName] = move(f_File);
            }
        }

        string //thank you chat-gpt uwu
            GetCurrentTimestamp()
        {
            auto now = chrono::system_clock::now();
            auto time_t_now = chrono::system_clock::to_time_t(now);
            auto local_time = *localtime(&time_t_now);

            stringstream ss;
            ss << put_time(&local_time, "%Y-%m-%d %H:%M:%S");

            auto since_epoch = now.time_since_epoch();
            auto milliseconds = chrono::duration_cast<chrono::milliseconds>(since_epoch).count() % 1000;

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
    };
}