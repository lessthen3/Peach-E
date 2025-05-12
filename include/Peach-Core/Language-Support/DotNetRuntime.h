#pragma once

///PeachCore
#include "../Managers/LogManager.h"

///DotNet
#include <dotnet/hostfxr.h>
#include <dotnet/coreclr_delegates.h>
#include <dotnet/nethost.h>

namespace PeachCore{

    struct ScriptInstance
    {
        void* OnUpdate = nullptr;
        void* OnStart = nullptr;
        void* OnExit = nullptr;
        void* ManagedInstance = nullptr; // ← the object created via Activator.CreateInstance

        string ScriptName;
    };

    struct DotNetRuntimeContext
    {
        void* HostFxrLib = nullptr;
        hostfxr_handle Context = nullptr;

        hostfxr_initialize_for_runtime_config_fn RuntimeInit = nullptr;
        hostfxr_get_runtime_delegate_fn GetDelegate = nullptr;
        hostfxr_close_fn Close = nullptr;
        load_assembly_and_get_function_pointer_fn LoadAssembly = nullptr;

        string RuntimePath;
        string MainAssembyPath;

        vector<ScriptInstance> Scripts;

        bool IsInitialized = false;
    };

    class DotNetRuntime
    {
    public:
        DotNetRuntime() = default;
        ~DotNetRuntime() = default;

        DotNetRuntimeContext m_DotNetRuntimeContext;

        bool
            InitializeRuntime()
        {

            return true;
        }


    };
}