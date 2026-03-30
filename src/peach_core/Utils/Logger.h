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

#define PEACH_LOGGER_DEFAULT_FLAGS PEACH_ALL_LOGS | PEACH_FLUSH_ERROR | PEACH_FLUSH_FATAL

#define PEACH_ARRAY_SIZE(x) sizeof(x) / sizeof(x[0]) 

#ifdef PEACH_DEBUG //TEMPORARY JUST HERE FOR NOW TESTING THE IDEA, SINCE DEBUG DOESNT ALWAYS MEAN USING TERMINAL
#define PEACH_USING_OS_TERMINAL
#endif

#define PEACH_FILENAME PeachExtractFilename(__FILE__)

#define PEACH_ASSERT(fp_Condition, fp_Message)                                              \
    do                                                                                      \
    {                                                                                       \
        if (!(fp_Condition))                                                                \
        {                                                                                   \
            std::fprintf(                                                                   \
                stderr,                                                                     \
                "[PEACH_ASSERT FAILED] %s\n  Condition : %s\n  Location  : %s:%d\n",       \
                (fp_Message), #fp_Condition, PEACH_FILENAME, __LINE__                      \
            );                                                                              \
            std::exit(-69420);                                                                   \
        }                                                                                   \
    } while (false)

#ifdef PEACH_PLATFORM_WINDOWS
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#endif

/// STL
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>

#include <unordered_map>
#include <format>
#include <memory>

#include <thread>
#include <optional>
#include <source_location>

///PeachCore
#include "../Utils/RingBuffer.h"
#include "../peach_api/LoggerFlags.h"


constexpr const int FATAL_SEGMENTATION_FAULT = -6969;

/// moody camel queue size uwu
constexpr const unsigned int MOODY_CAMEL_QUEUE_SIZE = 128;

namespace PeachCore {

    using namespace std; //this should be here so i dont affect anybody who links against peach

#if defined(PEACH_PLATFORM_WINDOWS) && defined(PEACH_USING_OS_TERMINAL)

    static bool
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
            cerr << ("Was not able to set console mode to allow windows to display ANSI escape codes") << "\n";
            return false;
        }
    }

#endif

    constexpr const char*
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

    constexpr std::string_view 
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

// #ifdef PEACH_USING_OS_TERMINAL

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
    
// #endif

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
        ~Logger(); ///XXX: Just copy and pasted the flushalllogs method because they have the assert at the beginning and wont work with premature exit

        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;
        Logger& operator=(Logger&&) = delete;

        /*
            needed for stack allocated Create(), allows for nrvo and also is kosher since move constructors play w the strict ownership model that is the foundation of the thread owning system uwu
            so the pattern is using optional return a nrvo Logger, and the thread thats using it calls Create() so this_thread::thread::id works properly ^_^
        */
        Logger(Logger&&) = default;

    public:
        static constexpr uint32_t MAX_NUMBER_OF_LOGS = 1024u;

        using LogBuffer = RingBuffer<LogMessage, MAX_NUMBER_OF_LOGS>;

        //////////////////////////////////////////////
        // Protected Constructor
        //////////////////////////////////////////////
    protected:
        Logger() = default;

        //////////////////////////////////////////////
        // Protected Class Members
        //////////////////////////////////////////////
    protected:
        unordered_map<string, ofstream> pm_LogFiles;

        unique_ptr<LogBuffer> pm_SnapshotBuffer = nullptr;

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
                const PEACH_LOGGER_FLAGS fp_Flags,
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
                const PEACH_LOGGER_FLAGS fp_Flags,
                const string& fp_DesiredOutputDirectory = ""
            )
        {
            unique_ptr<Logger> f_CreatedLogger(new Logger()); //this is dumb but std doesn't like my private constructor uwu!

            if (not f_CreatedLogger->Initialize(fp_DesiredLoggerName, fp_DesiredOutputDirectory, fp_Flags))
            {
                PrintError("Unable to initialize logger named: " + fp_DesiredLoggerName);
                return nullptr;
            }

            return f_CreatedLogger;
        }

        [[nodiscard]] static shared_ptr<Logger>
            CreateShared
            (
                const string& fp_DesiredLoggerName,
                const PEACH_LOGGER_FLAGS fp_Flags,
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
            UpdateActiveMask(const uint32_t fp_NewLogMask);

        const RingBuffer<LogMessage, MAX_NUMBER_OF_LOGS>&
            GetSnapshotBuffer()
            const noexcept
        {
            return *pm_SnapshotBuffer;
        }


        //////////////////// Flush All Logs ////////////////////

        [[nodiscard]] bool
            FlushAllLogs();

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
            );

        void
            Debug
            (
                const string& fp_Message,
                const string& fp_Sender
            );

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

        [[nodiscard]] bool
            CreateLogFile
            (
                const string& fp_FilePath,
                const string& fp_FileName
            );

        void
            CloseOpenLogFiles();

        [[nodiscard]] inline bool ///XXX: used for testing, this method should never call exit() for a production release, since all logging is hidden away from the game engine dev
            AssertThreadAccess(const string& fp_FunctionName) //we don't require a lock since this method guarantees only one thread is operating on any data within the Logger instance
            const;

        void
            ForceFlushAllLogs(); //called by functions that already do an AssertThreadAccess call in them uwu this is to avoid double calling OwO!
    };
}