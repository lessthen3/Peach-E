#include "../../include/Peach-Core/Language-Support/Lua/LuaScriptManager.h"

namespace PeachCore {

    auto LuaScriptManager::RunLuaScript(const std::string& fp_Script, const std::string& fp_ScriptName) {
        sol::state lua;
        lua.open_libraries(sol::lib::base);

        lua.script_file(fp_Script);

        if (lua["initialize"].valid()) {
            lua["initialize"]();
            LogManager::MainLogger().Debug("Successfully initialized script at: " + fp_ScriptName, "LuaScriptManager");
        }
        else
        { //only an error if the 
            LogManager::MainLogger().Error("Failed to locate Lua Initialize() function at: " + fp_ScriptName, "LuaScriptManager");
        }

        if (lua["update"].valid()) {
            lua["update"]();
        }
        else
        {
            LogManager::MainLogger().Error("Successfully located DLL at: " + fp_ScriptName, "LuaScriptManager");
        }
    }
}
