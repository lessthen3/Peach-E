#include "../../include/Peach-Editor/Editor/PeachConsole.h"

namespace PeachEditor {

    PeachConsole::PeachConsole()
    {
        Clear();
    }

    PeachConsole::~PeachConsole()
    {
        Clear();
    }

    void PeachConsole::Clear() 
    {
        buffer.clear();
        lineOffsets.clear();
        lineOffsets.push_back(0);
    }

    void PeachConsole::AddLog(const std::string& message) 
    {
        int old_size = buffer.size();

        buffer.append(message.c_str());

        for (int new_size = buffer.size(); old_size < new_size; old_size++)
        {
            if (buffer[old_size] == '\n')
            {
                lineOffsets.push_back(old_size + 1);
            }
        }
    }

    void PeachConsole::Draw(const char* title, const ImGuiWindowFlags& console_window_flags, bool* p_open)
    {
        ImGui::Begin(title, p_open, console_window_flags);

        if (ImGui::Button("Clear")) 
        {
            Clear();
        }

        ImGui::SameLine();
        bool copy_to_clipboard = ImGui::Button("Copy");
        ImGui::SameLine();
        filter.Draw("Filter", -100.0f);

        ImGui::Separator();
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
        if (copy_to_clipboard)
        {
            ImGui::LogToClipboard();
        }

        if (filter.IsActive())
        {
            const char* buf_begin = buffer.begin();
            const char* line = buf_begin;

            for (int line_no = 0; line != NULL; line_no++) 
            {
                const char* line_end = (line_no < lineOffsets.Size) ? buf_begin + lineOffsets[line_no] : NULL;

                if (filter.PassFilter(line, line_end))
                {
                    ImGui::TextUnformatted(line, line_end);
                }

                line = line_end && line_end[0] ? line_end + 1 : NULL;
            }
        }
        else
        {
            ImGui::TextUnformatted(buffer.begin());
        }

        if (scrollToBottom)
        {
            ImGui::SetScrollHereY(1.0f);
        }

        scrollToBottom = false;

        ImGui::EndChild();
        ImGui::End();
    }
}