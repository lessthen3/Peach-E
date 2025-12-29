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

///PeachCore
#include "../../Utils/Logger.h"

namespace PeachCore {


    //////////////////////////////////////////////
    // Console Struct
    //////////////////////////////////////////////


    struct PeachConsole 
    {
    public:
        PeachConsole();
        ~PeachConsole();

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
                const char* title, 
                bool* p_open = nullptr
            );

        void 
            SetScrollToBottom() 
        { 
            pm_IsScrollToBottom = true;
        }

    private:
        bool pm_IsScrollToBottom = false;
    };
}
