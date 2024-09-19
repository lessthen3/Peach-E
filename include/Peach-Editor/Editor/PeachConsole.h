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
#include "EditorObject.h"

namespace PeachEditor {

    struct PeachConsole : public EditorObject {
    public:
        PeachConsole();
        ~PeachConsole();

        void Clear();

        void AddLog(const std::string& message);

        void Draw(const char* title, const ImGuiWindowFlags& console_window_flags, bool* p_open = NULL);

        void SetScrollToBottom() { scrollToBottom = true; }

    private:
        ImGuiTextBuffer buffer;
        ImGuiTextFilter filter;
        ImVector<int> lineOffsets;
        bool scrollToBottom = false;
    };
}
