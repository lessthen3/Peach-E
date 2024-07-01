#pragma once

#include "angelscript.h"
#include <cassert>
#include <iostream>
#include <mutex>
#include <string>
#include <stdexcept>

#include "../../include/AngelScript/scriptstdstring/scriptstdstring.h"

namespace PeachCore {

    void PrintString(const std::string& fp_Msg);
    void PrintInt(const int& fp_Num);
    void PrintFloat(const float& fp_Num);

    struct ScriptEngineManager {
    public:
        static ScriptEngineManager& ScriptEngine() {
            static ScriptEngineManager instance;
            return instance;
        }

        static void MessageCallback(const asSMessageInfo* msg, void* param);

        asIScriptEngine* CreateScriptEngine();

        void ExecuteScript(asIScriptEngine* engine, const std::string& scriptCode);

        ~ScriptEngineManager();

    private:
        ScriptEngineManager();
        ScriptEngineManager(const ScriptEngineManager&) = delete;
        ScriptEngineManager& operator=(const ScriptEngineManager&) = delete;

        asIScriptEngine* engine;
        std::mutex mutex;

    };

}
