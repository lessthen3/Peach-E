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

#include <imgui.h>

#include <vector>
#include <string>
#include <sstream>
#include <map>
#include "EditorObject.h"

using namespace std;

namespace PeachEditor {

    struct ConsoleObject
    {
        unique_ptr<ImGuiTextBuffer> Buffer;
        unique_ptr<ImVector<int>> LineOffset;

        ConsoleObject(unique_ptr<ImGuiTextBuffer> fp_Buffer, unique_ptr<ImVector<int>> fp_LineOffset)
        {
            Buffer = move(fp_Buffer);
            LineOffset = move(fp_LineOffset);
        }
    };

    struct PeachConsole : public EditorObject 
    {
    public:
        PeachConsole();
        ~PeachConsole();

        void 
            ClearConsole
            (
                ConsoleObject* fp_CurrentTab = nullptr
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
                const ImGuiWindowFlags& console_window_flags, 
                bool* p_open = nullptr
            );

        void SetScrollToBottom() 
        { 
            pm_IsScrollToBottom = true;
        }

    private:
        //logs are all related to the current project game logs
        map<const string, ConsoleObject> pm_Buffers;

        ImGuiTextFilter pm_TextFilter;
        bool pm_IsScrollToBottom = false;

        ConsoleObject* pm_CurrentBufferObject = nullptr;
    };
}
