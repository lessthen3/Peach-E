#include "../../include/Peach-Core/Language-Support/Lua/LuaScriptManager.h"

namespace PeachCore {

    auto 
        LuaScriptManager::RunLuaScript(const std::string& fp_Script, const std::string& fp_ScriptName) 
    {
        sol::state lua;
        lua.open_libraries(sol::lib::base);

        lua.script_file(fp_Script);

        if (lua["initialize"].valid()) 
        {
            lua["initialize"]();
            LogManager::MainLogger().LogAndPrint("Successfully initialized script at: " + fp_ScriptName, "LuaScriptManager", "debug");
        }
        else
        { //only an error if the 
            LogManager::MainLogger().LogAndPrint("Failed to locate Lua Initialize() function at: " + fp_ScriptName, "LuaScriptManager", "error");
        }

        if (lua["update"].valid())
        {
            lua["update"]();
        }
        else
        {
            LogManager::MainLogger().LogAndPrint("Successfully located DLL at: " + fp_ScriptName, "LuaScriptManager", "error");
        }
    }
}
