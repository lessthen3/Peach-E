#include "../../include/Managers/ScriptEngineManager.h"

namespace PeachCore {

    void PrintString(const std::string& fp_Msg) {
        std::cout << fp_Msg << std::endl;
    }

    void PrintInt(const int& fp_Num) {
        std::cout << fp_Num << std::endl;
    }

    void PrintFloat(const float& fp_Num) {
        std::cout << fp_Num << std::endl;
    }

    ScriptEngineManager::ScriptEngineManager() : engine(nullptr) {}

    ScriptEngineManager::~ScriptEngineManager() {
        if (engine) {
            engine->ShutDownAndRelease();
        }
    }

    void ScriptEngineManager::MessageCallback(const asSMessageInfo* msg, void* param) {
        const char* type = "ERR ";
        if (msg->type == asMSGTYPE_WARNING)
            type = "WARN";
        else if (msg->type == asMSGTYPE_INFORMATION)
            type = "INFO";

        fprintf(stderr, "%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
    }

    asIScriptEngine* ScriptEngineManager::CreateScriptEngine() {
        std::lock_guard<std::mutex> lock(mutex);
        if (engine != nullptr) {
            return engine;
        }

        engine = asCreateScriptEngine();
        if (engine == nullptr) {
            throw std::runtime_error("Failed to create script engine.");
        }

        int r = engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
        if (r < 0) {
            throw std::runtime_error("Failed to set message callback.");
        }

        RegisterStdString(engine);

        // Register the print function for string
        r = engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(PrintString), asCALL_CDECL);
        if (r < 0) {
            throw std::runtime_error("Failed to register print function.");
        }

        // Register the print function for int
        r = engine->RegisterGlobalFunction("void print(const int &in)", asFUNCTION(PrintInt), asCALL_CDECL);
        if (r < 0) {
            throw std::runtime_error("Failed to register print function.");
        }

        // Register the print function for int
        r = engine->RegisterGlobalFunction("void print(const float &in)", asFUNCTION(PrintFloat), asCALL_CDECL);
        if (r < 0) {
            throw std::runtime_error("Failed to register print function.");
        }

        return engine;
    }

    void ScriptEngineManager::ExecuteScript(asIScriptEngine* engine, const std::string& scriptCode) {
        asIScriptModule* module = engine->GetModule("MyModule", asGM_ALWAYS_CREATE);
        module->AddScriptSection("script", scriptCode.c_str());

        int r = module->Build();
        if (r < 0) {
            throw std::runtime_error("Failed to build script module.");
        }

        asIScriptFunction* func = module->GetFunctionByDecl("void main()");
        if (func == nullptr) {
            throw std::runtime_error("Function 'void main()' not found.");
        }

        asIScriptContext* ctx = engine->CreateContext();
        ctx->Prepare(func);
        r = ctx->Execute();
        if (r != asEXECUTION_FINISHED) {
            std::cerr << "Execution failed with code: " << r << std::endl;
            if (r == asEXECUTION_EXCEPTION) {
                std::cerr << "Exception: " << ctx->GetExceptionString() << std::endl;
                std::cerr << "Function: " << ctx->GetExceptionFunction()->GetDeclaration() << std::endl;
                std::cerr << "Line: " << ctx->GetExceptionLineNumber() << std::endl;
            }
        }
        ctx->Release();
    }

}