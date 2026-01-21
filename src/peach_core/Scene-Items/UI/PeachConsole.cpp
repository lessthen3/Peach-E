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
#include "PeachConsole.h"

namespace PeachCore::PUI {

    //PeachConsole::PeachConsole() 
    //{
    //    //pm_PeachLogConsole = make_shared<Console>();
    //}

    [[nodiscard]] bool
        PeachConsole::ParseArguments(const string& fp_Args)
    {
        for (const char lv_CurrentChar : fp_Args)
        {

        }

        return true;
    }

    void
        PeachConsole::SetCommandList(vector<string>&& fp_CommandList)
    {
        pm_CommandList = move(fp_CommandList);
    }

    void
        PeachConsole::HookLogBuffer(shared_ptr<Logger::LogBuffer> fp_SnapshotBuffer)
    {

    }

    void 
        PeachConsole::ClearConsole
        (
            const string& fp_DesiredTab
        )
    {

    }

    void 
        PeachConsole::Draw
        (
            const string& title,
            bool& p_open
        )
    {

    }
}