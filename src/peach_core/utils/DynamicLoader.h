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
#include "debug/Logger.h"

#ifdef PEACH_PLATFORM_WINDOWS
    struct HINSTANCE__; // forward declare the opaque struct from windef.h, we do this to avoid pulling windows.h into every TU through the include chain
    #define DYNLIB_HANDLE HINSTANCE__* //XXX: pretty much just a typedef -> void* but windows is a special boy >:(
#else
    #define DYNLIB_HANDLE void*
#endif

namespace PeachCore::DynamicLoader {

    [[nodiscard]] DYNLIB_HANDLE
        LoadDynamicLibrary
        (
            const string& fp_DylibPath,
            Logger* logger
        );

    [[nodiscard]] bool
        UnloadLibrary
        (
            DYNLIB_HANDLE fp_LibraryHandle,
            Logger* logger
        );

    // Function to retrieve symbols (functions/variables) from the library
    [[nodiscard]] void* 
        GetSymbol
        (
            const string& fp_SymbolName, 
            DYNLIB_HANDLE fp_LibraryHandle,
            Logger* logger
        );
}
