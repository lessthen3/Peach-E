/*******************************************************************
 *                        Peach-E v0.0.1
 *         Created by Ranyodh Singh Mandur - 🍑 2024-2026
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#pragma once

///PeachCore
#include "Utils/Logger.h"

///Lua
extern "C"
{
    #include <lua/lua.h>
    #include <lua/lauxlib.h>
    #include <lua/lualib.h>
}

namespace PeachCore::Lua {

    struct ScriptInstance
    {
        string Name;
        int TableRef = LUA_NOREF;      // ref to the script’s table in the Lua registry
    };

    struct ScriptRuntime
    {
    public:
        ScriptRuntime() = default;
        ~ScriptRuntime() = default;

        ScriptRuntime(const ScriptRuntime&) = delete;
        ScriptRuntime& operator=(const ScriptRuntime&) = delete;

    public:
        bool Initialize(shared_ptr<Logger> fp_Logger);  
        void Shutdown();                 

    private:
        lua_State* pm_LuaState = nullptr;
        shared_ptr<Logger> lua_logger = nullptr;

    public:
        bool
            CreateInstanceFromBytecode
            (
                const uint8_t* fp_Bytecode,
                size_t fp_Size,
                const string& fp_Name,
                ScriptInstance& fp_OutInstance
            );

        bool
            CallOnUpdate
            (
                ScriptInstance& fp_ScriptInstance, 
                float fp_Delta
            );

        bool
            CallOnConstantUpdate
            (
                ScriptInstance& fp_ScriptInstance,
                float fp_Delta
            );

        bool
            CallOnEnter
            (
                ScriptInstance& fp_ScriptInstance
            );

        bool
            CallOnExit
            (
                ScriptInstance& fp_ScriptInstance
            );
    };
}//namespace PeachCore::Lua