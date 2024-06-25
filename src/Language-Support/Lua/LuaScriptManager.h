#pragma once

#include <sol/sol.hpp>
#include "../../Peach-core/Managers/LogManager.h"

namespace PeachCore {

    class LuaScriptManager
    {
    public:
        static LuaScriptManager& LuaScript() {
            static LuaScriptManager instance;
            return instance;
        }

        //sol::state Lua;
        auto RunLuaScript(const std::string& script, const std::string& fp_ScriptName);

    private:
        LuaScriptManager() = default;
        ~LuaScriptManager() = default;

        LuaScriptManager(const LuaScriptManager&) = delete;
        LuaScriptManager& operator=(const LuaScriptManager&) = delete;
    };
}