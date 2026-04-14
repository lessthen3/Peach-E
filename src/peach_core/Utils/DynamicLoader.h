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
#include "Logger.h"

///STL
#include <fmt/format.h>
#include <filesystem>

namespace PeachCore {
    //Imagine if windows was posix compliant, what a world that'd be >O<
    #ifdef PEACH_PLATFORM_WINDOWS
        //XXX: we do this to avoid weird stuff w unicode and ansi strings, LoadLibrary is just a macro and since its a preprocessor thing it can cause runtime trouble
        // UTF8 -> wide string helper for LoadLibraryW
        inline HINSTANCE
            LoadLibraryUTF8(const char* fp_Path)
        {
            int f_SizeNeeded = MultiByteToWideChar(CP_UTF8, 0, fp_Path, -1, NULL, 0);
            wstring f_WidePath(f_SizeNeeded, 0);
            MultiByteToWideChar(CP_UTF8, 0, fp_Path, -1, &f_WidePath[0], f_SizeNeeded);
            return LoadLibraryW(f_WidePath.c_str()); 
        }
    #endif
}//namespace PeachCore

#ifdef PEACH_PLATFORM_WINDOWS
    #define DYNLIB_HANDLE HINSTANCE //XXX: pretty much just a typedef -> void* but windows is a special boy >:(
    #define DYNLIB_LOAD(fp_Path) PeachCore::LoadLibraryUTF8(fp_Path)
    #define DYNLIB_GETSYM GetProcAddress
    #define DYNLIB_UNLOAD FreeLibrary
#else
    #include <dlfcn.h>
    #define DYNLIB_HANDLE void*
    #define DYNLIB_LOAD(fp_Path) dlopen(fp_Path, RTLD_LAZY)
    #define DYNLIB_GETSYM dlsym
    #define DYNLIB_UNLOAD dlclose
#endif

namespace PeachCore {

    //////////////////////////////////////////////
    // Dynamic Library Loader Struct
    //////////////////////////////////////////////
    struct DynamicLoader
    {
    public:
        DynamicLoader() = delete;
        ~DynamicLoader() = default;

        static DYNLIB_HANDLE
            LoadDynamicLibrary
            (
                const string& fp_DylibPath,
                Logger* logger
            )
        {
            if (not filesystem::exists(fp_DylibPath) and filesystem::is_regular_file(fp_DylibPath))
            {
                logger->Error(fmt::format("Library path does not exist: '{}'", fp_DylibPath), "DynamicLoader");
                return nullptr;
            }

            //logger->Debug(fmt::format("Successfully located DLL at: {}", fp_DylibPath), "DynamicLoader");

            DYNLIB_HANDLE f_LibraryHandle = DYNLIB_LOAD(fp_DylibPath.c_str());

            if (not f_LibraryHandle)
            {
                logger->Error(fmt::format("Failed to load library: '{}',  Error: '{}'", fp_DylibPath, GetLastErrorAsString()), "DynamicLoader");
                return nullptr;
            }

            logger->Info(fmt::format("Library loaded successfully: '{}'", fp_DylibPath), "DynamicLoader");

            return f_LibraryHandle;
        }

        static bool
            UnloadLibrary
            (
                DYNLIB_HANDLE fp_LibraryHandle,
                Logger* logger
            )
        {
            if (not fp_LibraryHandle)
            {
                logger->Error("Tried passing a nullptr reference to a DYNLIB_HANDLE inside GetSymbol()", "DynamicLoader");
                return false;
            }

            if (not DYNLIB_UNLOAD(fp_LibraryHandle))
            {
                logger->Error(fmt::format("Failed to unload library. Error: '{}'", GetLastErrorAsString()), "DynamicLoader");
                return false;
            }
            
            logger->Info("Library unloaded successfully", "DynamicLoader");

            return true; //Unloaded Library Successfully! >W<
        }

        // Function to retrieve symbols (functions/variables) from the library
        static void* 
            GetSymbol
            (
                const string& fp_SymbolName, 
                DYNLIB_HANDLE fp_LibraryHandle,
                Logger* logger
            )
        {
            void* symbol = nullptr;

            if (not fp_LibraryHandle)
            {
                logger->Error("Tried passing a nullptr reference to a DYNLIB_HANDLE inside GetSymbol()", "DynamicLoader");
                return nullptr;
            }

            symbol = (void*)DYNLIB_GETSYM(fp_LibraryHandle, fp_SymbolName.c_str());

            if (not symbol)
            {
                logger->Error(fmt::format("Failed to locate symbol: '{}', Error: '{}'", fp_SymbolName, GetLastErrorAsString()), "DynamicLoader");
                return nullptr; //its already nullptr but its nice to be explicit here
            }
            
            logger->Debug(fmt::format("Symbol located: '{}'", fp_SymbolName), "DynamicLoader");

            return symbol;
        }

    private:
        // Helper function to get the error message string
        static string 
            GetLastErrorAsString()
        {
            #if defined(_WIN32) or defined(_WIN64) //wtf windows are u okay
                // Windows error message
                DWORD errorMessageID = ::GetLastError();

                if (errorMessageID == 0)
                {
                    return "No error found UwU"; // No error message has been recorded
                }

                LPSTR messageBuffer = nullptr;
                size_t size = FormatMessageA
                (
                    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL, 
                    errorMessageID, 
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                    (LPSTR)&messageBuffer, 
                    0, 
                    NULL
                );

                string message(messageBuffer, size);
                LocalFree(messageBuffer);

                return message;
            #else
                // POSIX error message
                return string(dlerror());
            #endif
        }
    };
}
