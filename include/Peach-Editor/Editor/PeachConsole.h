/*******************************************************************
 *                                        Peach Editor v0.0.7
 *                           Created by Ranyodh Mandur - � 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                 Peach Editor is an open source editor for Peach-E
********************************************************************/
#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <map>
#include "EditorObject.h"

using namespace std;

namespace PeachEditor {

    struct PeachConsole : public EditorObject 
    {
    public:
        PeachConsole();
        ~PeachConsole();

        void 
            ClearConsole
            (
                const string& fp_DesiredTab
            );
        void
            CreateLogBuffers();
        void 
            AddLog
            (
                const string& fp_Message,
                const string& fp_Sender
            );

        void 
            Draw
            (
                const char* title, 
                bool* p_open = nullptr
            );

        void SetScrollToBottom() 
        { 
            pm_IsScrollToBottom = true;
        }

    private:
        //logs are all related to the current project game logs
        map<const string, vector<string>> pm_Buffers = 
        {
            {"everything", vector<string>()},
            {"main_thread", vector<string>()},
            {"render_thread", vector<string>()},
            {"audio_thread", vector<string>()},
            {"resource_thread", vector<string>()},

            {"editor_warn", vector<string>()},
            {"editor_error", vector<string>()}
        };

        bool pm_IsScrollToBottom = false;

    };
}
