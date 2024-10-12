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
    }

    PeachConsole::~PeachConsole()
    {
        for (auto& _log : pm_Buffers)
        {
            _log.second.clear(); //clear all vectors owo
        }
    }

    void 
        PeachConsole::ClearConsole
        (
            const string& fp_DesiredTab
        )
    {

    }

    void 
        PeachConsole::AddLog
        (
            const string& fp_Message,
            const string& fp_Sender
        ) 
    {
        pm_Buffers.at(fp_Sender).push_back(fp_Message);
    }

    void 
        PeachConsole::Draw
        (
            const char* title, 
            bool* p_open
        )
    {

    }
}