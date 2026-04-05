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
#include "LuaScriptRuntime.h"

namespace PeachCore::Lua {

    // --- C wrapper that Lua can call ---
    static int
        Lua_PEACH_LogInfo(lua_State* fp_L)
    {
        const char* f_Message = luaL_checkstring(fp_L, 1);
        const char* f_Sender = luaL_checkstring(fp_L, 2);

        if (lua_gettop(fp_L) >= 2 && lua_isstring(fp_L, 2))
        {
            f_Sender = lua_tostring(fp_L, 2);
        }

        PEACH_LogInfo(f_Message, f_Sender);

        return 0;
    }

    bool 
        ScriptRuntime::Initialize(shared_ptr<Logger> fp_Logger)
    {
        if (not fp_Logger)
        {
            PRINT_ERROR("Tried to pass nullptr ref for Logger -> Lua::ScriptRuntime UwU");
            return false;
        }

        lua_logger = fp_Logger;

        // Create Lua state
        pm_LuaState = luaL_newstate();

        if (not pm_LuaState)
        {
            lua_logger->Error("Failed to create Lua state", "Lua::ScriptRuntime");
            
            return false;
        }

        ////////////////////////////////////////////// Avoid OS and IO libs  //////////////////////////////////////////////

        luaL_requiref(pm_LuaState, "_G", luaopen_base, 1); lua_pop(pm_LuaState, 1);
        luaL_requiref(pm_LuaState, "package", luaopen_package, 1); lua_pop(pm_LuaState, 1);
        luaL_requiref(pm_LuaState, "string", luaopen_string, 1); lua_pop(pm_LuaState, 1);
        luaL_requiref(pm_LuaState, "table", luaopen_table, 1); lua_pop(pm_LuaState, 1);
        luaL_requiref(pm_LuaState, "math", luaopen_math, 1); lua_pop(pm_LuaState, 1);
        luaL_requiref(pm_LuaState, "utf8", luaopen_utf8, 1); lua_pop(pm_LuaState, 1);
        luaL_requiref(pm_LuaState, "debug", luaopen_debug, 1); lua_pop(pm_LuaState, 1);

        // Register Peach API functions into Lua global namespace

        // Option 1: register as a global function
        // Lua: PEACH_LogInfo("msg", "sender")
        lua_register(pm_LuaState, "PEACH_LogInfo", Lua_PEACH_LogInfo);

        // Option 2 (fancier): put them into a table "Peach"
        //   Peach.LogInfo("msg", "sender")
        // Uncomment if you prefer:
        /*
        luaL_Reg peachFuncs[] = {
            { "LogInfo", Lua_PEACH_LogInfo },
            { nullptr, nullptr }
        };
        luaL_newlib(m_L, peachFuncs);
        lua_setglobal(m_L, "Peach");
        */

        lua_logger->Info("Lua runtime initialized and Peach API bound", "Lua::ScriptRuntime");
        
        return true;
    }

    void 
        ScriptRuntime::Shutdown()
    {
        if (pm_LuaState)
        {
            lua_close(pm_LuaState);
            pm_LuaState = nullptr;
        }

        lua_logger->Info("Lua runtime shutdown", "Lua::ScriptRuntime");
    }

    bool 
        ScriptRuntime::CallOnEnter(ScriptInstance& fp_ScriptInstance, PEACH_NODE nodeID)
    {
        if (not pm_LuaState or fp_ScriptInstance.TableRef == LUA_NOREF)
        {

            return false;
        }

        // Push table onto stack
        lua_rawgeti(pm_LuaState, LUA_REGISTRYINDEX, fp_ScriptInstance.TableRef); // stack: [ table ]

        // Get Init function: table.Init
        lua_getfield(pm_LuaState, -1, "on_enter"); // stack: [ table, Init ]
        if (not lua_isfunction(pm_LuaState, -1))
        {
            lua_pop(pm_LuaState, 2); // pop Init (non-func) + table
            return false; // no Init is fine, just skip
        }

        // Push 'self' (if you want) or nodeID
        // Example: pass nodeID
        lua_pushinteger(pm_LuaState, static_cast<lua_Integer>(nodeID)); // stack: [ table, Init, nodeID ]

        // Call: 1 arg, 0 returns
        int status = lua_pcall(pm_LuaState, 1, 0, 0);
        if (status != LUA_OK)
        {
            const char* err = lua_tostring(pm_LuaState, -1);
            lua_logger->Error(string("Lua error calling Init on '") + fp_ScriptInstance.Name + "': " + (err ? err : "<unknown>"), "Lua::ScriptRuntime");

            lua_pop(pm_LuaState, 1); // pop error
            lua_pop(pm_LuaState, 1); // pop table (if left)

            return false;
        }

        // Pop table (still on stack)
        lua_pop(pm_LuaState, 1);
        return true;
    }

    bool 
        ScriptRuntime::CallOnUpdate(ScriptInstance& inst, float dt)
    {
        if (not pm_LuaState or inst.TableRef == LUA_NOREF)
        {

            return false;
        }

        lua_rawgeti(pm_LuaState, LUA_REGISTRYINDEX, inst.TableRef); // [ table ]
        lua_getfield(pm_LuaState, -1, "Update");                    // [ table, Update ]

        if (not lua_isfunction(pm_LuaState, -1))
        {
            lua_pop(pm_LuaState, 2); // [ ]
            return false; // no Update defined, just skip
        }

        lua_pushnumber(pm_LuaState, dt); // [ table, Update, dt ]

        int status = lua_pcall(pm_LuaState, 1, 0, 0);
        if (status != LUA_OK)
        {
            const char* err = lua_tostring(pm_LuaState, -1);
            lua_logger->Error(string("Lua error calling Update on '") + inst.Name + "': " + (err ? err : "<unknown>"), "Lua::ScriptRuntime");

            lua_pop(pm_LuaState, 1); // pop error
            lua_pop(pm_LuaState, 1); // pop table (if still present)

            return false;
        }

        lua_pop(pm_LuaState, 1); // pop table
        return true;
    }

    bool 
        ScriptRuntime::CreateInstanceFromBytecode
        (
            const uint8_t* fp_Bytecode,
            size_t fp_Size,
            const string& fp_Name,
            ScriptInstance& fp_OutInstance
        )
    {
        if (not pm_LuaState)
        {
            lua_logger->Error("CreateInstanceFromBytecode called with null Lua state", "Lua::ScriptRuntime");
            return false;
        }

        // Load the chunk (function) from raw bytes
        int loadStatus = luaL_loadbuffer
        (
            pm_LuaState,
            reinterpret_cast<const char*>(fp_Bytecode),
            fp_Size,
            fp_Name.c_str()
        );

        if (loadStatus != LUA_OK)
        {
            const char* err = lua_tostring(pm_LuaState, -1);
            lua_logger->Error(string("Failed to load Lua chunk '") + fp_Name + "': " + (err ? err : "<unknown>"), "Lua::ScriptRuntime");
            lua_pop(pm_LuaState, 1); // pop error

            return false;
        }

        // Stack now: [ chunkFunction ]
        // Call it: we expect it to return one value (the table with Init/Update/etc)
        int callStatus = lua_pcall(pm_LuaState, 0, 1, 0);
        if (callStatus != LUA_OK)
        {
            const char* err = lua_tostring(pm_LuaState, -1);
            lua_logger->Error(string("Error running Lua chunk '") + fp_Name + "': " + (err ? err : "<unknown>"), "Lua::ScriptRuntime");
            lua_pop(pm_LuaState, 1); // pop error

            return false;
        }

        // Now top of stack should be the returned table
        if (not lua_istable(pm_LuaState, -1))
        {
            lua_logger->Error(string("Lua script '") + fp_Name + "' did not return a table", "Lua::ScriptRuntime");
            lua_pop(pm_LuaState, 1);

            return false;
        }

        fp_OutInstance.TableRef = luaL_ref(pm_LuaState, LUA_REGISTRYINDEX);
        fp_OutInstance.Name = fp_Name;

        return true;
    }

}//namespace PeachCore::Lua
