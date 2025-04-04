#pragma once

#include <sol/sol.hpp>
#include "../Managers/LogManager.h"

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