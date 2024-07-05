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

        // After all engines are released
        asUnprepareMultithread();
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

        // Before creating engines
        asPrepareMultithread();


        engine = asCreateScriptEngine();
        if (engine == nullptr) {
            throw std::runtime_error("Failed to create script engine.");
        }

        int r = engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
        if (r < 0) {
            throw std::runtime_error("Failed to set message callback.");
        }

        RegisterStdString(engine);

        // Register atomic types
        RegisterAtomicTypes(engine);
   

        RegisterPrintFunction(engine);

        

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

    void ScriptEngineManager::RegisterPrintFunction(asIScriptEngine* engine)
    {
        // Register the print function for string
        int r = engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(PrintString), asCALL_CDECL);
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
    }

    void ScriptEngineManager::RegisterAtomicTypes(asIScriptEngine* engine) {
        // Helper lambda to register atomic types
        auto registerAtomicType = [&](const char* typeName, auto exampleValue) {
            using T = decltype(exampleValue);
            int r = engine->RegisterObjectType(typeName, sizeof(std::atomic<T>), asOBJ_VALUE | asOBJ_APP_CLASS_CDAK); assert(r >= 0);
            r = engine->RegisterObjectBehaviour(typeName, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION([](void* mem) { new(mem) std::atomic<T>(); }), asCALL_CDECL_OBJFIRST); assert(r >= 0);
            r = engine->RegisterObjectBehaviour(typeName, asBEHAVE_DESTRUCT, "void f()", asFUNCTION([](std::atomic<T>* mem) { mem->~atomic(); }), asCALL_CDECL_OBJFIRST); assert(r >= 0);
            r = engine->RegisterObjectMethod(typeName, (std::string("const ") + typeName + " get() const").c_str(), asFUNCTION([](const std::atomic<T>* atomic) { return atomic->load(); }), asCALL_CDECL_OBJFIRST); assert(r >= 0);
            r = engine->RegisterObjectMethod(typeName, (std::string("void set(const ") + typeName + "& in)").c_str(), asFUNCTION([](std::atomic<T>* atomic, T value) { atomic->store(value); }), asCALL_CDECL_OBJFIRST); assert(r >= 0);
            r = engine->RegisterObjectMethod(typeName, (typeName + std::string(" increment()")).c_str(), asFUNCTION([](std::atomic<T>& atomic) { return atomic.fetch_add(1) + 1; }), asCALL_CDECL_OBJFIRST); assert(r >= 0);
            r = engine->RegisterObjectMethod(typeName, (typeName + std::string(" decrement()")).c_str(), asFUNCTION([](std::atomic<T>& atomic) { return atomic.fetch_sub(1) - 1; }), asCALL_CDECL_OBJFIRST); assert(r >= 0);
            };


        // Register atomic types
        registerAtomicType("atomic_int", int{});
        registerAtomicType("atomic_uint", unsigned{});
        registerAtomicType("atomic_long", long{});
        registerAtomicType("atomic_ulong", unsigned long{});
        registerAtomicType("atomic_llong", long long{});
        registerAtomicType("atomic_ullong", unsigned long long{});
        //registerAtomicType("atomic_bool", bool{});
        registerAtomicType("atomic_char", char{});
        registerAtomicType("atomic_uchar", unsigned char{});
        registerAtomicType("atomic_short", short{});
        registerAtomicType("atomic_ushort", unsigned short{});
        registerAtomicType("atomic_float", float{});
        registerAtomicType("atomic_double", double{});
        // Add more types as needed
    }

}