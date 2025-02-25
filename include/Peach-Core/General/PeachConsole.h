/*******************************************************************
 *                                             Peach-E v0.1
 *                           Created by Ranyodh Mandur - � 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                         Peach-E is an open-source game engine
********************************************************************/
#pragma once

#include <vector>
#include <sstream>
#include <memory>

#include "../Managers/LogManager.h"


using namespace std;

namespace PeachCore {

    struct PeachConsole 
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
            Draw
            (
                const char* title, 
                bool* p_open = nullptr
            );

        void 
            SetScrollToBottom() 
        { 
            pm_IsScrollToBottom = true;
        }

        shared_ptr<Console>
            GetConsoleLogger()
        {
            return pm_PeachLogConsole;
        }

    private:
        bool pm_IsScrollToBottom = false;
        shared_ptr<Console> pm_PeachLogConsole = nullptr;
    };
}
