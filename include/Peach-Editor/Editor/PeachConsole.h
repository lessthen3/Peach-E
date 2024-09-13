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
