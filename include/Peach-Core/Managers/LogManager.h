/*******************************************************************
 *                                             Peach-E v0.1
 *                           Created by Ranyodh Mandur - � 2024
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

namespace PeachCore {

    static string
        CreateColouredText
        (
            const string& fp_SampleText,
            const string& fp_DesiredColour
        )
    {
        //////////////////// Regular Colours ////////////////////

        if (fp_DesiredColour == "black")
        {
            return "\x1B[30m" + fp_SampleText + "\033[0m";
        }
        else if (fp_DesiredColour == "red")
        {
            return "\x1B[31m" + fp_SampleText + "\033[0m";
        }
        else if (fp_DesiredColour == "green")
        {
            return "\x1B[32m" + fp_SampleText + "\033[0m";
        }
        else if (fp_DesiredColour == "yellow")
        {
            return "\x1B[33m" + fp_SampleText + "\033[0m";
        }
        else if (fp_DesiredColour == "blue")
        {
            return "\x1B[34m" + fp_SampleText + "\033[0m";
        }
        else if (fp_DesiredColour == "magenta")
        {
            return "\x1B[35m" + fp_SampleText + "\033[0m";
        }
        else if (fp_DesiredColour == "cyan")
        {
            return "\x1B[36m" + fp_SampleText + "\033[0m";
        }
        else if (fp_DesiredColour == "white")
        {
            return "\x1B[37m" + fp_SampleText + "\033[0m";
        }

        //////////////////// Bright Colours ////////////////////

        else if (fp_DesiredColour == "bright black")
        {
            return "\x1B[90m" + fp_SampleText + "\033[0m";
        }
        else if (fp_DesiredColour == "bright red")
        {
            return "\x1B[91m" + fp_SampleText + "\033[0m";
        }
        else if (fp_DesiredColour == "bright green")
        {
            return "\x1B[92m" + fp_SampleText + "\033[0m";
        }
        else if (fp_DesiredColour == "bright yellow")
        {
            return "\x1B[93m" + fp_SampleText + "\033[0m";
        }
        else if (fp_DesiredColour == "bright blue")
        {
            return "\x1B[94m" + fp_SampleText + "\033[0m";
        }
        else if (fp_DesiredColour == "bright magenta")
        {
            return "\x1B[95m" + fp_SampleText + "\033[0m";
        }
        else if (fp_DesiredColour == "bright cyan")
        {
            return "\x1B[96m" + fp_SampleText + "\033[0m";
        }
        else if (fp_DesiredColour == "bright white")
        {
            return "\x1B[97m" + fp_SampleText + "\033[0m";
        }

        //////////////////// Just Return the Input Text Unaltered Otherwise ////////////////////

        else
        {
            return fp_SampleText;
        }
    }

    static void
        Print
        (
            const string& fp_Message,
            const string& fp_DesiredColour = "white"
        )
    {
        cout << CreateColouredText(fp_Message, fp_DesiredColour) << "\n";
    }

    static void
        PrintError
        (
            const string& fp_Message,
            const string& fp_DesiredColour = "red"
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
                pm_MainThreadLogBuffer->push_back({ fp_Message, fp_Sender });
            }
            else if (fp_ThreadName == "render_thread")
            {
                pm_RenderThreadLogBuffer->push_back({ fp_Message, fp_Sender });
            }
            else if (fp_ThreadName == "audio_thread")
            {
                pm_AudioThreadLogBuffer->push_back({ fp_Message, fp_Sender });
            }
            else if (fp_ThreadName == "resource_thread")
            {
                pm_ResourceThreadLogBuffer->push_back({ fp_Message, fp_Sender });
            }
            else if (fp_ThreadName == "physics_thread")
            {
                pm_PhysicsThreadLogBuffer->push_back({ fp_Message, fp_Sender });
            }
            else if (fp_ThreadName == "network_thread")
            {
                pm_NetworkThreadLogBuffer->push_back({ fp_Message, fp_Sender });
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
        ~LogManager()
        {
            FlushAllLogs();  // Ensure all logs are flushed before destruction

            for (auto& _f : pm_LogFiles)
            {
                if (_f.second.is_open())
                {
                    _f.second.close();
                }
            }
        }
    //////////////////////////////////////////////
    // Public Constructor
    //////////////////////////////////////////////
    public:
        LogManager() = default;

    //////////////////////////////////////////////
    // Protected Class Members
    //////////////////////////////////////////////
    protected:
        bool pm_HasBeenInitialized = false;

        map<string, ofstream> pm_LogFiles;

        string pm_LoggerName = "No_Logger_Name";
        string pm_CurrentWorkingDirectory = "nothing";

        shared_ptr<Console> pm_Console = nullptr;

    //////////////////////////////////////////////
    // Public Methods
    //////////////////////////////////////////////
    public:
        bool
            Initialize
            (
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
                    PrintError("An error occurred inside LogManager: " + static_cast<string>(ex.what()));
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
                    PrintError("An error occurred inside LogManager: " + static_cast<string>(ex.what()));
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

        //////////////////// Flush All Logs ////////////////////

        void
            FlushAllLogs()
        {
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
                const string& fp_LogLevel,
                const string& fp_ThreadName
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

            pm_Console->AddLog("[" + pm_LoggerName + "]: " + fp_Message + "\n", pm_LoggerName, fp_ThreadName);

            return f_LogEntry;
        }

        void
            LogAndPrint
            (
                const string& fp_Message,
                const string& fp_Sender,
                const string& fp_LogLevel,
                const string& fp_ThreadName
            )
        {
            string f_LogEntry = fp_LogLevel + ": [" + fp_Sender + "] " + fp_Message + "\n";

            // Log to console
            if (fp_LogLevel == "trace")
            {
                Print(Log(fp_Message, fp_Sender, fp_LogLevel, fp_ThreadName), "bright white");
            }
            else if (fp_LogLevel == "debug")
            {
                Print(Log(fp_Message, fp_Sender, fp_LogLevel, fp_ThreadName), "bright blue");
            }
            else if (fp_LogLevel == "info")
            {
                Print(Log(fp_Message, fp_Sender, fp_LogLevel, fp_ThreadName), "bright green");
            }
            else if (fp_LogLevel == "warn")
            {
                Print(Log(fp_Message, fp_Sender, fp_LogLevel, fp_ThreadName), "bright yellow");
            }
            else if (fp_LogLevel == "error")
            {
                PrintError(Log(fp_Message, fp_Sender, fp_LogLevel, fp_ThreadName), "red"); //not bright oooo soo dark and moody and complex and hard to reach and engage with ><
            }
            else if (fp_LogLevel == "fatal")
            {
                PrintError(Log(fp_Message, fp_Sender, fp_LogLevel, fp_ThreadName), "magenta");
            }
            else
            {
                Print(Log("Did not input a valid option for log level in LogAndPrint()", "LogManager", "error", fp_ThreadName));
                Print(Log(fp_Message, fp_Sender, fp_LogLevel, fp_ThreadName));
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
                cerr << "Failed to open log file: " << fp_FileName << "\n";
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
    };
}