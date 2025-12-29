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
#include "LuaScriptRuntime.h"

namespace PeachCore {

    void 
        LuaScriptRuntime::RunLuaScript
        (
            const string& fp_Script, 
            const string& fp_ScriptName
        )
    {
        //sol::state lua;
        //lua.open_libraries(sol::lib::base);

        //lua.script_file(fp_Script);

        //if (lua["initialize"].valid()) 
        //{
        //    lua["initialize"]();
        //    LogManager::MainLogger().LogAndPrint("Successfully initialized script at: " + fp_ScriptName, "LuaScriptManager", "debug");
        //}
        //else
        //{ //only an error if the 
        //    LogManager::MainLogger().LogAndPrint("Failed to locate Lua Initialize() function at: " + fp_ScriptName, "LuaScriptManager", "error");
        //}

        //if (lua["update"].valid())
        //{
        //    lua["update"]();
        //}
        //else
        //{
        //    LogManager::MainLogger().LogAndPrint("Successfully located DLL at: " + fp_ScriptName, "LuaScriptManager", "error");
        //}
    }
}
