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

///DotNet

#ifdef PEACH_PLATFORM_FREEBSD //freebsd still on dotnet 9
    #include <dotnet9/hostfxr.h>
    #include <dotnet9/coreclr_delegates.h>
#else
    #include <dotnet10/hostfxr.h>
    #include <dotnet10/coreclr_delegates.h>
#endif

///STL
#include <vector>

namespace PeachCore {

    struct ScriptInstance
    {
        void* OnEnter = nullptr;
        void* OnUpdate = nullptr;
        void* OnConstantUpdate = nullptr;
        void* OnExit = nullptr;
        void* ManagedInstance = nullptr; // ← the object created via Activator.CreateInstance

        string ScriptName;
    };

    struct DotnetContext
    {
        hostfxr_handle HostFxr = nullptr;

        hostfxr_initialize_for_runtime_config_fn RuntimeInit = nullptr;
        hostfxr_get_runtime_delegate_fn GetDelegate = nullptr;
        hostfxr_close_fn Close = nullptr;

        load_assembly_and_get_function_pointer_fn LoadAssembly = nullptr;

        load_assembly_and_get_function_pointer_fn CreateScriptInstance = nullptr;
        load_assembly_and_get_function_pointer_fn ReleaseScriptInstance = nullptr;

        load_assembly_and_get_function_pointer_fn CallOnEnter = nullptr;
        load_assembly_and_get_function_pointer_fn CallOnUpdate = nullptr;
        load_assembly_and_get_function_pointer_fn CallOnConstantUpdate = nullptr;
        load_assembly_and_get_function_pointer_fn CallOnExit = nullptr;

        string RuntimePath;
        string MainAssembyPath;

        vector<ScriptInstance> Scripts;

        bool IsInitialized = false;
    };
}//namespace PeachCore

namespace PeachCore{

}//namespace PeachCore