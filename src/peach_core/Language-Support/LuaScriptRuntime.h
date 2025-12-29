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
#pragma once

#include "../peach_api/PeachAPI.h"
#include "../Utils/Logger.h"

namespace PeachCore {

    struct LuaScriptComponent
    {

    };

    class LuaScriptRuntime
    {
    public:
        LuaScriptRuntime() = default;
        ~LuaScriptRuntime() = default;

        LuaScriptRuntime(const LuaScriptRuntime&) = delete;
        LuaScriptRuntime& operator=(const LuaScriptRuntime&) = delete;

    public:

        //sol::state Lua;
        void 
            RunLuaScript
            (
                const string& script, 
                const string& fp_ScriptName
            );
    };
}