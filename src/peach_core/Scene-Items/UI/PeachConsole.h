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

///STL
#include <memory>
#include <variant>

///PeachCore
#include "../../Utils/Logger.h"
#include "../PeachNode.h"

namespace PeachCore::PUI {

    using ArgType = variant<string, int64_t, uint64_t, double>;
    using ParsedArgument = pair<string, string>;

    constexpr int64_t INVALID_COMMAND = 0;
    constexpr size_t STRING_DISPLAY_BUFFER_MAX_SIZE = 1024; //MAYBE: idk might make peach console templated but thats annoying so I can add an argument for the page size ig uwu

    struct PeachConsoleCommand
    {
        const string Symbol; //-G, --debug
        const string MetaVariable; //[generator], [help idk]
        const string HelpMessage;

        explicit
            PeachConsoleCommand
            (
                const string& fp_Symbol, 
                const string& fp_MetaVariable,
                const string& fp_HelpMessage = ""
            ) 
            : 
                Symbol(fp_Symbol), 
                MetaVariable(fp_MetaVariable),
                HelpMessage(fp_HelpMessage)
        {}
    };

    struct PeachConsoleTab
    {
        string Name;
        RingBuffer<string, STRING_DISPLAY_BUFFER_MAX_SIZE> pm_StringDisplayBuffer;
    };

    //////////////////////////////////////////////
    // Console Struct
    //////////////////////////////////////////////

    struct PeachConsole : public Node
    {
    private:
        bool pm_IsScrollToBottom{ false }; //scroll to bottom whenever new log appears or command is input
        bool pm_ShouldEchoCommand{ true }; //echos command input when true uwu

        unordered_map<string, Logger> pm_Loggers;

        unordered_map<string, PeachConsoleCommand> pm_CommandList;

        PeachConsoleTab pm_MainConsoleScreen;

    public:
        virtual ~PeachConsole() = default;

        PeachConsole
        (
            const string& fp_NodeName,
            const uint32_t fp_Index,
            const uint32_t fp_Generation,
            const PeachNodeFlags fp_Flags = PeachNodeFlags::NONE
            )
            :
            Node(fp_NodeName, fp_Index, fp_Generation, fp_Flags, NodeType::Console)
        {}

        //WIP NEED TO LOCK THE THREAD SO THAT WE CAN SAFELY QUERY THE LOG BUFFERS SINCE THEY CAN BE WRITTEN TOO WHILE
        //bool
        //    QueryLogBufferByLevel
        //    (
        //        const ThreadName fp_NameOfLogBuffer,
        //        const uint8_t fp_DesiredLogLevelQuery
        //    )
        //{
        //    //switch (fp_NameOfLogBuffer)
        //    //{
        //    //    case ThreadName::MainThread: pm_MainThreadLogSnapshotBuffer.emplace_back(fp_Message, fp_Sender); break;
        //    //    case ThreadName::RenderThread: pm_RenderThreadLogSnapshotBuffer.emplace_back(fp_Message, fp_Sender); break;
        //    //    case ThreadName::AudioThread: pm_AudioThreadLogSnapshotBuffer.emplace_back(fp_Message, fp_Sender); break;
        //    //    case ThreadName::ResourceThread: pm_ResourceThreadLogBuffer.emplace_back(fp_Message, fp_Sender); break;
        //    //    case ThreadName::PhysicsThread: pm_PhysicsThreadLogBuffer.emplace_back(fp_Message, fp_Sender); break;
        //    //    case ThreadName::NetworkThread: pm_NetworkThreadLogBuffer.emplace_back(fp_Message, fp_Sender); break;
        //    //    default:
        //    //        PRINT_ERROR("Attempted to Log to an invalid thread log buffer: Did you check for any typos when calling the Log() function?\n\tSender: " + fp_Sender + "\n\tMessage: " + fp_Message);
        //    //}

        //    return true;
        //}

        [[nodiscard]] Logger*
            CreateLogger(const string& fp_LoggerName, PEACH_LOGGER_FLAGS fp_LoggerFlags); //should create logger flags as a typedef extern C enum to interop uwu future ryan: ye did it uwu

        void 
            ClearConsole
            (
                const string& fp_DesiredTab
            );

        void
            GetFormattedHelpString();

        [[nodiscard]] bool
            ParseArguments
            (
                const string& fp_CommandString,
                vector<ParsedArgument>& fp_ParsedArguments
            );

        void
            AddCommand
            (
                const string& fp_Symbol,
                const string& fp_MetaVariable,
                const string& fp_HelpMessage = ""
            );

        void 
            SetScrollToBottom() 
        { 
            pm_IsScrollToBottom = true;
        }
    };
}
