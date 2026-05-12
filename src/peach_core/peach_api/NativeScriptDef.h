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
#ifndef PEACH_NATIVE_SCRIPT_DEF_HG
#define PEACH_NATIVE_SCRIPT_DEF_HG

#include "NodeDef.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    // void* UserData is the per-instance state slot
    // C:   malloc'd struct, free it in OnShutdown
    // Rust: Box::into_raw() on init, Box::from_raw() + drop in OnShutdown
    // C++: new SomeState(), delete in OnShutdown — no vtable, no NativeScript base needed
    typedef void (*PEACH_OnInitializeFn)    (PEACH_NodeID fp_NodeID, void* fp_UserData);
    typedef void (*PEACH_OnUpdateFn)        (PEACH_NodeID fp_NodeID, double fp_DeltaTime, void* fp_UserData);
    typedef void (*PEACH_OnConstantUpdateFn)(PEACH_NodeID fp_NodeID, double fp_FixedDelta, void* fp_UserData);
    typedef void (*PEACH_OnShutdownFn)      (PEACH_NodeID fp_NodeID, void* fp_UserData);

    typedef struct{
        PEACH_OnInitializeFn     OnInitialize;
        PEACH_OnUpdateFn         OnUpdate;
        PEACH_OnConstantUpdateFn OnConstantUpdate;
        PEACH_OnShutdownFn       OnShutdown;
        void* UserData; // engine stores this, passes it back every call
    } PEACH_ScriptDef;

    // Dynamic plugins export this symbol — engine calls it after dlopen to get the def
    // For multi-script dylibs, pass the script name; single-script dylibs ignore it
    typedef PEACH_ScriptDef(*PEACH_GetScriptDefFn)(const char* fp_ScriptName);

#ifdef __cplusplus
}
#endif

#endif /*PEACH_NATIVE_SCRIPT_DEF_HG*/