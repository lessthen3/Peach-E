#pragma once


#include <vector>
#include <string>
#include <sstream>
#include "PeachEObject.h"

namespace PeachEngine {

    struct PeachEConsole : public PeachEObject {
    public:
        PeachEConsole() {
            Clear();
        }

        void Clear();

        void AddLog(const std::string& message);

        void Draw(const char* title, bool* p_open = NULL);

        void SetScrollToBottom() { scrollToBottom = true; }

    private:
        //ImGuiTextBuffer buffer;
        //ImGuiTextFilter filter;
        //ImVector<int> lineOffsets;
        bool scrollToBottom = false;
    };
}
