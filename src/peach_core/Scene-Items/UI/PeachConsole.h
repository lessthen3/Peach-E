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

///STL
#include <memory>
#include <variant>

///PeachCore
#include "../../Utils/Logger.h"
#include "../PeachNode.h"

namespace PeachCore::PUI {

    using ArgType = variant<string, int64_t, uint64_t, double>;

    constexpr int64_t INVALID_COMMAND = 0;

    //////////////////////////////////////////////
    // Console Struct
    //////////////////////////////////////////////

    struct PeachConsole : public Node
    {
    public:
        virtual ~PeachConsole() = default;
        PeachConsole(const string& fp_NodeName, const PeachNodeID fp_NodeID, const uint8_t fp_Flags) : Node(fp_NodeName, fp_NodeID, fp_Flags, NodeType::Console) {}

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
        //    //        PrintError("Attempted to Log to an invalid thread log buffer: Did you check for any typos when calling the Log() function?\n\tSender: " + fp_Sender + "\n\tMessage: " + fp_Message);
        //    //}

        //    return true;
        //}

        void 
            ClearConsole
            (
                const string& fp_DesiredTab
            );

        void 
            Draw
            (
                const string& title, 
                bool& p_open
            );

        [[nodiscard]] bool
            ParseArguments(const string& fp_Args);

        void
            HookLogBuffer(shared_ptr<Logger::LogBuffer> fp_SnapshotBuffer);

        void
            SetCommandList(vector<string>&& fp_CommandList);

        void 
            SetScrollToBottom() 
        { 
            pm_IsScrollToBottom = true;
        }

    private:
        bool pm_IsScrollToBottom{ false }; //scroll to bottom whenever new log appears

        unordered_map<uint64_t, shared_ptr<Logger::LogBuffer>> pm_HookedBuffers;

        vector<string> pm_CommandList;
    };
}
