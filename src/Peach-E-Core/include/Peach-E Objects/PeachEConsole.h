#pragma once

#include <imgui.h>

#include <vector>
#include <string>
#include <sstream>
#include "PeachEObject.h"

namespace PeachEditor {

    struct PeachEConsole : public PeachEObject {
    public:
        PeachEConsole();
        ~PeachEConsole();

        void Clear();

        void AddLog(const std::string& message);

        void Draw(const char* title, bool* p_open = NULL);

        void SetScrollToBottom() { scrollToBottom = true; }

    private:
        ImGuiTextBuffer buffer;
        ImGuiTextFilter filter;
        ImVector<int> lineOffsets;
        bool scrollToBottom = false;
    };
}
