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
#include "../../include/Peach-Editor/Editor/PeachConsole.h"

namespace PeachEditor {

    PeachConsole::PeachConsole()
    {
        ClearConsole();
    }

    PeachConsole::~PeachConsole()
    {
        ClearConsole();

        if (pm_CurrentBufferObject)
        {
            pm_CurrentBufferObject = nullptr; //this ptr isnt really responsible for any creation, i just think this is good form
            //also the peach console's life span is equal to the program lifetime so yeah doesn't really matter tbh
        }
    }

    void
        PeachConsole::CreateLogBuffers()
    {
        unique_ptr<ImGuiTextBuffer> f_EveryBuffer = make_unique<ImGuiTextBuffer>();
        unique_ptr<ImGuiTextBuffer> f_MainThreadLogsBuffer = make_unique<ImGuiTextBuffer>();
        unique_ptr<ImGuiTextBuffer> f_RenderThreadLogsBuffer = make_unique<ImGuiTextBuffer>();
        unique_ptr<ImGuiTextBuffer> f_AudioThreadLogsBuffer = make_unique<ImGuiTextBuffer>();
        unique_ptr<ImGuiTextBuffer> f_ResourceThreadLogsBuffer = make_unique<ImGuiTextBuffer>();

        //Internal Logs warn and error
        unique_ptr<ImGuiTextBuffer> f_EditorWarnLogsBuffer = make_unique<ImGuiTextBuffer>();
        unique_ptr<ImGuiTextBuffer> f_EditorErrorLogsBuffer = make_unique<ImGuiTextBuffer>();


        unique_ptr<ImVector<int>> f_EveryLineOffset = make_unique<ImVector<int>>();
        unique_ptr<ImVector<int>> f_MainLineOffset = make_unique<ImVector<int>>();
        unique_ptr<ImVector<int>> f_RenderLineOffset = make_unique<ImVector<int>>();
        unique_ptr<ImVector<int>> f_AudioLineOffset = make_unique<ImVector<int>>();
        unique_ptr<ImVector<int>> f_ResourceLineOffset = make_unique<ImVector<int>>();

        unique_ptr<ImVector<int>> f_EditorWarnLineOffset = make_unique<ImVector<int>>();
        unique_ptr<ImVector<int>> f_EditorErrorLineOffset = make_unique<ImVector<int>>();
        
        pm_Buffers.insert({ "everything", ConsoleObject(move(f_EveryBuffer), move(f_EveryLineOffset)) });
        pm_Buffers.insert({ "main_thread", ConsoleObject(move(f_MainThreadLogsBuffer), move(f_MainLineOffset)) });
        pm_Buffers.insert({ "render_thread", ConsoleObject(move(f_RenderThreadLogsBuffer), move(f_RenderLineOffset)) });
        pm_Buffers.insert({ "audio_thread", ConsoleObject(move(f_AudioThreadLogsBuffer), move(f_AudioLineOffset)) });
        pm_Buffers.insert({ "resource_thread", ConsoleObject(move(f_ResourceThreadLogsBuffer), move(f_ResourceLineOffset)) });

        pm_Buffers.insert({ "editor_warn", ConsoleObject(move(f_EditorWarnLogsBuffer), move(f_EditorWarnLineOffset)) });
        pm_Buffers.insert({ "editor_error", ConsoleObject(move(f_EditorErrorLogsBuffer), move(f_EditorErrorLineOffset)) });

        
    }

    void 
        PeachConsole::ClearConsole
        (
            ConsoleObject* fp_CurrentTab
        )
    {
        if (fp_CurrentTab)
        {
            fp_CurrentTab->Buffer->clear();
            fp_CurrentTab->LineOffset->clear();
            fp_CurrentTab->LineOffset->push_back(0);
        }
        else
        {
            for (auto& _obj : pm_Buffers)
            {
                _obj.second.Buffer->clear();
                _obj.second.LineOffset->clear();
                _obj.second.LineOffset->push_back(0);
            }
        }
    }

    void 
        PeachConsole::AddLog
        (
            const string& fp_Message,
            const string& fp_Sender
        ) 
    {
        int old_size = pm_Buffers.at("everything").Buffer->size();

        pm_Buffers.at("everything").Buffer->append(fp_Message.c_str());
        pm_Buffers.at(fp_Sender).Buffer->append(fp_Message.c_str());

        for (int new_size = pm_Buffers.at("everything").Buffer->size(); old_size < new_size; old_size++)
        {
            if ((*(pm_Buffers.at("everything").Buffer))[old_size] == '\n')
            {
                pm_Buffers.at("everything").LineOffset->push_back(old_size + 1);
                pm_Buffers.at(fp_Sender).LineOffset->push_back(old_size + 1);
            }
        }
    }

    void 
        PeachConsole::Draw
        (
            const char* title, 
            const ImGuiWindowFlags& console_window_flags, 
            bool* p_open
        )
    {
        ImGui::Begin(title, p_open, console_window_flags);

        if (ImGui::Button("Clear")) 
        {
            ClearConsole(pm_CurrentBufferObject);
        }

        ImGui::SameLine();
        bool copy_to_clipboard = ImGui::Button("Copy");

        ImGui::SameLine();
        // Begin tab bar
        if (ImGui::BeginTabBar("##ConsoleTabs", ImGuiTabBarFlags_None))
        {
            //IMPORTANT: CORRESPONG TO THE CURRENT GAME PROJECTS LOGS (INTERNAL LOGS FOR NOW WHILE WE TEST)
            if (ImGui::BeginTabItem("All"))
            {
                pm_CurrentBufferObject = &pm_Buffers.at("everything");
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Main Thread"))
            {
                pm_CurrentBufferObject = &pm_Buffers.at("main_thread");
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Rendering"))
            {
                pm_CurrentBufferObject = &pm_Buffers.at("render_thread");  // Assuming you have a separate pm_EverythingBuffer for the audio thread
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Audio"))
            {
                pm_CurrentBufferObject = &pm_Buffers.at("audio_thread");  // Assuming you have a separate pm_EverythingBuffer for the audio thread
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Resources"))
            {
                pm_CurrentBufferObject = &pm_Buffers.at("resource_thread");  // Assuming you have a separate pm_EverythingBuffer for the audio thread
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Warn"))
            {
                pm_CurrentBufferObject = &pm_Buffers.at("editor_warn");  // Assuming you have a separate pm_EverythingBuffer for the audio thread
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Errors"))
            {
                pm_CurrentBufferObject = &pm_Buffers.at("editor_error");  // Assuming you have a separate pm_EverythingBuffer for the audio thread
                ImGui::EndTabItem();
            }
            // More tabs for other threads or log levels
            ImGui::EndTabBar();
        }

        ImGui::Separator();

        ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
        if (copy_to_clipboard)
        {
            ImGui::LogToClipboard();
        }

        if (pm_TextFilter.IsActive())
        {
            const char* buf_begin = pm_CurrentBufferObject->Buffer->begin();
            const char* line = buf_begin;

            for (int line_no = 0; line != NULL; line_no++) 
            {
                const char* line_end = (line_no < pm_CurrentBufferObject->LineOffset->Size) ? buf_begin + (*(pm_CurrentBufferObject->LineOffset))[line_no] : NULL;

                if (pm_TextFilter.PassFilter(line, line_end))
                {
                    ImGui::TextUnformatted(line, line_end);
                }

                line = line_end && line_end[0] ? line_end + 1 : NULL;
            }
        }
        else
        {
            ImGui::TextUnformatted(pm_CurrentBufferObject->Buffer->begin());
        }

        if (pm_IsScrollToBottom)
        {
            ImGui::SetScrollHereY(1.0f);
        }

        pm_IsScrollToBottom = false;

        ImGui::EndChild();
        ImGui::End();
    }
}