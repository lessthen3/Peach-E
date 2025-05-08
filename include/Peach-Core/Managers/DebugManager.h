/*******************************************************************
 *                                             Peach-E v0.0.1
 *                           Created by Ranyodh Mandur - 🍑 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                     Peach-E is a free open source game engine
********************************************************************/
#pragma once

//WE WANT THIS CLASS TO PARSE THROUGH LOGS AND PROVIDE TOOLS TO INTROSPECT INTO THE ENGINE AND ITS ACTIVITIES

//THE SIMPLEST WAY I CAN THINK OF DOING THIS IS TO PROVIDE A WAY TO QUERY LOGS EFFICIENTLY AND POSSIBLY OTHER USEFUL DEBUGGING TOOLS

//FOR CREATING SANDBOX ENVIRONMENTS AND ISOLATING CERTAIN PARTS OF THE EXECUTION

namespace PeachCore {

    class DebugManager 
    {
    public:
        static DebugManager& get_single()
        {
            static DebugManager debug_manager;
            return debug_manager;
        }

    private:
        DebugManager() = default;
        ~DebugManager() = default;

        DebugManager(const DebugManager&) = delete;
        DebugManager& operator=(const DebugManager&) = delete;
    };

}